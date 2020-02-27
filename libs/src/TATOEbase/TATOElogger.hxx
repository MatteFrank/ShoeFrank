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
#include <algorithm>


namespace details{
    struct immutable_tag{};
    struct fleeting_tag{};
    struct normal_tag{};
}// namespace details

struct TATOElogger{
    
    struct header{
        long cursor;
    };
    
    struct sub_header{
        long start_cursor;
        long cursor;
        int status;
        std::size_t order;
    };
    
private:
    std::stringstream buffer_m;
    std::vector< header > header_mc; //poor optional
    std::vector< sub_header > sub_header_mc;
    long end_cursor_m{0};
    // std::size_t last_immutable_order{0};
    
public:
    template< std::size_t Order, class T = details:: normal_tag,
    typename std::enable_if_t< (Order > 0), std::nullptr_t> = nullptr,
    typename std::enable_if_t< std::is_same<T, details::immutable_tag>::value ||
    std::is_same<T, details::fleeting_tag>::value ||
    std::is_same<T, details::normal_tag>::value , std::nullptr_t > = nullptr >
    TATOElogger& add_sub_header( char const * input_ph )
    {
        
        auto reverse_find = [this](auto predicate_p)
        {
            auto reversed_iterator = std::find_if( sub_header_mc.rbegin(), sub_header_mc.rend(),
                                                  [&predicate_p](sub_header const& sh_p)
                                                  { return predicate_p(sh_p);  } );
            
            return ( reversed_iterator + 1 ).base();
        };
        
        
        int status = status_value(T{});
        
        if( !sub_header_mc.empty() )
        {
            auto previous_iterator = sub_header_mc.end() - 1 ;
            int path_order = ( previous_iterator->order > Order ) ?  10 :
            ( previous_iterator->order < Order ? -10 : 0 );
            int path_status = previous_iterator->status;
            int path = path_order + path_status;
            
            switch(path){
                case -11:
                {
                    //case -11 -- previous_sub_header is of lower order and fleeting
                    break;
                }
                case -10:
                {
                    //case : -10 -- previous_sub_header is of lower order and immutable
                    status = status_value(details::immutable_tag{});
                    break;
                }
                case -9:
                {
                    //case : -9 -- previous_sub_header is of lower order and normal
                    break;
                }
                case -1:
                {
                    //case : -1 -- previous_sub_header is of same order and fleeting
                    buffer_m.seekp( previous_iterator->start_cursor );
                    break;
                }
                case 0:
                {
                    //case : 0 -- previous_sub_header is of same order and immutable
                    auto iterator = reverse_find(  [](sub_header const& sh_p){ return sh_p.order < Order ; } );
                    if( iterator != sub_header_mc.begin()-1 && iterator->status == 0 ){ status = 0; }
                    break;
                }
                case 1:
                {
                    //case 1: previous_sub_header is of same order and normal"
                    buffer_m.seekp( previous_iterator->cursor );
                    break;
                }
                case 9:
                {
                    //case 9: previous_sub_header is of higher order and fleeting
                    auto same_order_iterator = reverse_find( [](sub_header const& sh_p){ return (sh_p.order == Order) ;} );
                    if( same_order_iterator == sub_header_mc.begin()-1 ){
                        same_order_iterator = sub_header_mc.begin();
                    }
                    
                    auto immutable_iterator = std::find_if( same_order_iterator, sub_header_mc.end(),
                                                           [](sub_header const & sh_p){ return sh_p.status == 0 ; } );
                    auto last_immutable_iterator = immutable_iterator;
                    while(  immutable_iterator != sub_header_mc.end()  )
                    {
                        last_immutable_iterator = immutable_iterator;
                        auto immutable_order = last_immutable_iterator->order;
                        immutable_iterator = std::find_if( last_immutable_iterator + 1, sub_header_mc.end(),
                                                          [&immutable_order](sub_header const & sh_p){ return (sh_p.order == immutable_order) && (sh_p.status == 0) ; } );
                    }
                    
                    if( last_immutable_iterator != sub_header_mc.end() ) {
                        auto immutable_order = last_immutable_iterator->order;
                        auto iterator = reverse_find( [&immutable_order](sub_header const& sh_p){ return (sh_p.order == immutable_order) ;} );
                        
                        if( iterator->status == -1 ){
                            buffer_m.seekp( iterator->start_cursor );
                            sub_header_mc.erase( iterator, sub_header_mc.end() );
                        }
                        else{
                            buffer_m.seekp( iterator->cursor );
                            sub_header_mc.erase( iterator + 1, sub_header_mc.end() );
                        }
                    }
                    else{
                        if( same_order_iterator->status == -1 ){
                            buffer_m.seekp( same_order_iterator->start_cursor );
                            sub_header_mc.erase( same_order_iterator , sub_header_mc.end() );
                        }
                        else{
                            buffer_m.seekp( same_order_iterator->cursor );
                            sub_header_mc.erase( same_order_iterator + 1 , sub_header_mc.end() );
                        }
                    }
                    
                    break;
                }
                case 10:
                {
                    //case :  10 -- previous_sub_header is of higher order and immutable
                    auto iterator = reverse_find(  [](sub_header const& sh_p){ return sh_p.order == Order ; } );
                    if( iterator != sub_header_mc.begin()-1 ){
                        auto lower_iterator = reverse_find(  [](sub_header const& sh_p){ return sh_p.order < Order ; } );
                        if( lower_iterator != sub_header_mc.begin()-1 && lower_iterator->status == 0 ){ status = 0; }
                    }
                    break;
                }
                case 11:
                {
                    // case : 11 -- previous_sub_header is of higher order and normal ;
                    auto same_order_iterator = reverse_find( [](sub_header const& sh_p){ return (sh_p.order == Order) ;} );
                    if( same_order_iterator == sub_header_mc.begin()-1 ){
                        same_order_iterator = sub_header_mc.begin();
                    }
                    
                    auto immutable_iterator = std::find_if( same_order_iterator, sub_header_mc.end(),
                                                           [](sub_header const & sh_p){ return sh_p.status == 0 ; } );
                    auto last_immutable_iterator = immutable_iterator;
                    while(  immutable_iterator != sub_header_mc.end()  )
                    {
                        last_immutable_iterator = immutable_iterator;
                        auto immutable_order = last_immutable_iterator->order;
                        immutable_iterator = std::find_if( last_immutable_iterator + 1, sub_header_mc.end(),
                                                          [&immutable_order](sub_header const & sh_p){ return (sh_p.order == immutable_order) && (sh_p.status == 0) ; } );
                    }
                    
                    if( last_immutable_iterator != sub_header_mc.end() ) {
                        auto immutable_order = last_immutable_iterator->order;
                        auto iterator = reverse_find( [&immutable_order](sub_header const& sh_p){ return (sh_p.order == immutable_order) ;} );
                        
                        if( iterator->status == -1 ){
                            buffer_m.seekp( iterator->start_cursor );
                            sub_header_mc.erase( iterator, sub_header_mc.end() );
                        }
                        else{
                            buffer_m.seekp( iterator->cursor );
                            sub_header_mc.erase( iterator + 1, sub_header_mc.end() );
                        }
                    }
                    else{
                        if( same_order_iterator->status == -1 ){
                            buffer_m.seekp( same_order_iterator->start_cursor );
                            sub_header_mc.erase( same_order_iterator , sub_header_mc.end() );
                        }
                        else{
                            buffer_m.seekp( same_order_iterator->cursor );
                            sub_header_mc.erase( same_order_iterator + 1 , sub_header_mc.end() );
                        }
                    }
                    
                    break;
                }
                default: break;
            }
        }
        
        long start_cursor = buffer_m.tellp();
        buffer_m << "---- " << input_ph << " ----\n";
        sub_header_mc.push_back( sub_header{start_cursor, buffer_m.tellp(), status, Order} );
        end_cursor_m = buffer_m.tellp();
        
        return *this;
    }
    
private:
    int status_value(details::normal_tag) const { return 1;}
    int status_value(details::immutable_tag) const { return 0;}
    int status_value(details::fleeting_tag) const { return -1;}
    
    
public:
    TATOElogger& add_header( char const * input_ph )
    {
        auto frozen_sub_header = [this]()
        {
            return std::any_of( sub_header_mc.begin(), sub_header_mc.end(),
                               [](sub_header const & sh_p)
                               { return sh_p.status == 0; } );
        };
        
        if( !sub_header_mc.empty() ) {
            auto cursor = frozen_sub_header() ?
            ( sub_header_mc.back().status == 0 ? end_cursor_m : sub_header_mc.back().cursor ) :
            ( header_mc.empty() ? 0 : header_mc.back().cursor ) ;
            buffer_m.seekp( cursor );
        }
        buffer_m << "==== " << input_ph << " ====\n";
        header_mc.clear();
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
        if ( !sub_header_mc.empty() ) {
            sub_header_mc.back().status = 0;
        }
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
                                                              [](sub_header const & sh_p){ return sh_p.status == 0; } );   };
        
        long cursor;
        if( !sub_header_mc.empty() ) {
            cursor = frozen_sub_header() ? ( sub_header_mc.back().status == 0 ? end_cursor_m : sub_header_mc.back().cursor ) :
            ( header_mc.empty() ? sub_header_mc.back().cursor : header_mc.back().cursor ) ;
        }
        else{
            cursor = header_mc.empty() ? end_cursor_m : header_mc.back().cursor ;
        }
        output.erase(cursor);
        std::cout << output;
    }
    
};





#endif
