//
// Plot tot residual + Fit for FIRST data
//
/** TATOElogger class
 
 \author A. Sécher
 */

//
//File      : TATOElogger.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 19/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


#ifndef _TATOElogger_HXX
#define _TATOElogger_HXX


#include <sstream>
#include <iostream>
#include <vector>
#include <utility>

struct TATOElogger{
    
    struct header{
        long cursor;
    };
    
    struct sub_header{
        std::size_t order;
        long cursor;
    };
    
private:
    std::stringstream buffer_m;
    std::vector< header > header_mc;
    std::vector< sub_header > sub_header_mc;
    long end_cursor_m{0};
    
public:
    template<std::size_t Order, typename std::enable_if_t< (Order > 0) , std::nullptr_t > = nullptr>
    TATOElogger& add_sub_header( char const * input_ph )
    {
        if( !sub_header_mc.empty() && !(Order > sub_header_mc.back().order ) )
        {
            auto find_last_sub_header = [this](std::size_t order_p)
            {
                return ( std::find_if( sub_header_mc.rbegin(), sub_header_mc.rend(), [&order_p](sub_header const& sh_p){ return order_p == sh_p.order;  }  ) + 1 ).base();
            };
            
            auto is_order_found = [this](std::size_t order_p)
            {
                return std::any_of( sub_header_mc.rbegin(), sub_header_mc.rend(), [&order_p](sub_header const& h_p){ return order_p == h_p.order ; } );
            };
            
            auto reset_to = [this, &find_last_sub_header]( std::size_t order_p )
            {
                auto last_sub_header_h = find_last_sub_header( order_p );
                
                buffer_m.seekp( last_sub_header_h->cursor );
                end_cursor_m = last_sub_header_h->cursor ;
                sub_header_mc.erase( last_sub_header_h, sub_header_mc.end() );
            };
            
            is_order_found(Order) ?  reset_to( Order ) : clear() ;
        }
        
        buffer_m << "---- " << input_ph << " ----\n";
        sub_header_mc.push_back( sub_header{Order, static_cast<long>( buffer_m.tellp() )} );
        end_cursor_m = buffer_m.tellp();
        
        return *this;
    }
    
    //    template<class T, typename std::enable_if_t< std::is_same<T, details::immutable>::value , std::nullptr_t > = nullptr >
    //     TATOElogger& add_sub_header( char const * input_ph )
    //     {
    //         buffer_m << "---- " << input_ph << " ----\n";
    //         sub_header_mc.push_back( sub_header{ 0, buffer_m.tellp()}  );
    //         end_cursor_m = buffer_m.tellp();
    //         return *this;
    //     }
    
    
    TATOElogger& add_header( char const * input_ph )
    {
        auto frozen_sub_header = [this](){ return std::any_of( sub_header_mc.begin(), sub_header_mc.end(),
                                                              [](sub_header const & sh_p){ return sh_p.order == 0; } );   };
        
        if( !sub_header_mc.empty() ) {
            auto cursor = frozen_sub_header() ? ( sub_header_mc.back().cursor ) :
            ( header_mc.empty() ? 0 : header_mc.back().cursor ) ;
            buffer_m.seekp( cursor );
        }
        buffer_m << "==== " << input_ph << " ====\n";
        header_mc.push_back( header{ buffer_m.tellp() }  );
        
        end_cursor_m = buffer_m.tellp();
        sub_header_mc.clear();
        
        return *this;
    }
    
    template<class T>
    TATOElogger& operator<<(const T& t_p)
    {
        buffer_m << t_p;
        end_cursor_m = buffer_m.tellp();
        return *this;
    }
    
    void freeze()
    {
        sub_header_mc.empty() ? sub_header_mc.push_back( sub_header{ 0, end_cursor_m } ) : void(sub_header_mc.back().order = 0);
    }
    
    void clear()
    {
        std::stringstream{}.swap(buffer_m);
        header_mc.clear();
        sub_header_mc.clear();
        end_cursor_m = 0;
    }
    
    void output() const
    {
        auto output = buffer_m.str();
        auto frozen_sub_header = [this](){ return std::any_of( sub_header_mc.begin(), sub_header_mc.end(),
                                                              [](sub_header const & sh_p){ return sh_p.order == 0; } );   };
        
        long cursor;
        if( !sub_header_mc.empty() ) {
            cursor = frozen_sub_header() ? ( sub_header_mc.back().order == 0 ? end_cursor_m : sub_header_mc.back().cursor ) :
            ( header_mc.empty() ? 0 : header_mc.back().cursor ) ;
        }
        else{
            cursor = header_mc.empty() ? end_cursor_m : header_mc.back().cursor ;
        }
        output.erase(cursor);
        std::cout << output;
    }
    
};







#endif
