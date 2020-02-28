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

#include "TATOEutilities.hxx"

namespace details{
    
    template<class T, class Enabler> struct need_formatting_impl : std::false_type {};
    template<class T>
    struct need_formatting_impl< T, std::enable_if_t< std::is_arithmetic<T>::value && !std::is_same<T, char>::value > > : std::true_type {};
    
    template<class T>
    using need_formatting = need_formatting_impl<T, void>;
    
    
    template<class T>
    inline std::size_t string_size(T const & t_p);
    template<class T>
    inline std::size_t string_size(T* t_p);

    
    template<>
    inline std::size_t string_size(char const * input_ph)
    {
        return input_ph ? strlen(input_ph) : 0 ;
    };
    template<>
    inline std::size_t string_size(std::string const & s_p)
    {
        return s_p.size();
    };
    template<std::size_t N>
    inline std::size_t string_size(char (&) [N])
    {
        return N;
    };
    
    
    template<class T, class ... Ts>
    inline std::size_t string_size(T const & t_p,  Ts const & ... ts_p )
    {
        return string_size(t_p) + string_size(ts_p...);
    }
    
    template<class T, class U>
    std::size_t string_size( T const & t_p, U const & u_p )
    {
        return string_size(t_p) + string_size(u_p);
    }
    
    
    
    template<class ... Ts>
    struct concatenate_impl;
    
    template<class T, class ... Ts>
    struct concatenate_impl<T, Ts...>
    {
        static void concatenate(std::string & result_p,  T && t_p, Ts && ... ts_p )
        {
            result_p += std::forward<T>(t_p);
            concatenate_impl<Ts...>::concatenate(result_p, std::forward<Ts>(ts_p)...);
        }
    };
    
    template<class T, class U>
    struct concatenate_impl<T, U>
    {
        static void concatenate(std::string & result_p, T && t_p, U && u_p)
        {
            result_p += std::forward<T>(t_p);
            result_p += std::forward<U>(u_p);
        }
    };
    
    template<class ... Ts>
    std::string concatenate(Ts && ... ts_p)
    {
        std::string result;
        result.reserve( string_size(ts_p ...) );
        concatenate_impl<Ts...>::concatenate(result, ts_p...);
        return result;
    }
    



template<class T>
struct node
{
    
private:
    T t_m;
    std::vector< std::unique_ptr<node> > children_mc;
    node* parent_mh;
    
public:
    node(T&& t_p, node* parent_ph) :
    t_m{ std::move(t_p) },
    parent_mh{ parent_ph }
    {
        children_mc.reserve(5);
    }
    node(const node&) = delete;
    node& operator=(const node&) = delete;
    node(node&&) = delete;
    node& operator=(node&&) = delete;
    ~node() = default;
    
    
public:
    node * add_child(T&& t_p)
    // requires : isValid_m == true, should not be accessible if invalid node
    {
        children_mc.emplace_back( new node{std::move(t_p) , this} );
        return children_mc.back().get();
    }
    
    T & get_value() { return t_m; }
    T const & get_value() const { return t_m; }
    bool is_childless() const { return children_mc.empty(); }
    
    std::vector< std::unique_ptr<node> >& get_children() { return children_mc; }
    std::vector< std::unique_ptr<node> > const & get_children() const { return children_mc; }
    
    node const * get_parent() const { return parent_mh; }
    node * get_parent()  { return parent_mh; }
    
    template<class Predicate>
    node * find_parent(  Predicate p_p )
    {
        return const_cast<node*>( static_cast<node const*>(this)->find_parent( p_p ) );
    }
    template<class Predicate>
    node const * find_parent( Predicate p_p )  const
    {
        node const * current_node_h = this->get_parent();
        while( current_node_h && !p_p(current_node_h) ){ current_node_h = current_node_h->get_parent(); }
        return current_node_h;
    }
    
private:
    std::vector< std::unique_ptr<node> >& get_siblings()
    {
        return const_cast<std::vector< std::unique_ptr<node> >&>( static_cast<node const*>(this)->get_siblings() );
    }
    std::vector< std::unique_ptr<node> > const & get_siblings() const
    {
        return this->get_parent()->get_children();
    }
    
public:
    node const * get_ancestor() const
    {
        node const * current_node_h = this;
        while( current_node_h->get_parent() ){ current_node_h = current_node_h->get_parent(); }
        return current_node_h;
    }
    node * get_ancestor()
    {
        node * current_node_h = this;
        while( current_node_h->get_parent() ){ current_node_h = current_node_h->get_parent(); }
        return current_node_h;
    }
    
    template<class F>
    void walk_and_apply(F f_p)
    {
        f_p(*this);
        if( !is_childless() ){
            for(auto& child_h : children_mc ){
                child_h->walk_and_apply( f_p );
            }
        }
    }
    
    template<class F>
    void reverse_walk_and_apply(F f_p)
    {
        // std::cout << "function: " << get_value().data;
        if( this->get_parent() ){
            auto& sibling_c = get_siblings();
            for( auto & sibling : sibling_c){ f_p(*sibling) ; }
            get_parent()->reverse_walk_and_apply( f_p );
        }
        else{ f_p(*this); }
    }
    
    template<class F>
    void lineage_walk(F f_p)
    {
        f_p(*this);
        if( get_parent() ){
            get_parent()->lineage_walk( f_p );
        }
    }
};


}// namespace details


struct TATOElogger{
    
    struct header{
        std::string data;
        int status;
        std::size_t cursor;
        std::size_t order;
        
        header(std::string&& s_p, int status_p, std::size_t order_p) :
        data{std::move(s_p)},
        status{status_p},
        cursor{ data.size() },
        order{order_p} {}
    };
    
    using node_type = details::node<header>;
    
private:
    std::vector< std::unique_ptr<node_type> > root_mc;
    node_type * current_node_mh;
    
public:
    template< class T = details::non_removable_tag,
              typename std::enable_if_t< std::is_same<T, details::immutable_tag>::value ||
                                         std::is_same<T, details::fleeting_tag>::value ||
                                         std::is_same<T, details::non_removable_tag>::value , std::nullptr_t > = nullptr >
    TATOElogger& add_root_header( char const * input_ph )
    {
        root_mc.emplace_back( new node_type{
            header{
                details::concatenate("====", input_ph, "====\n"),
                status_value(T{}),
                0
            },
            nullptr
        }  );
        
        current_node_mh = root_mc.back().get();
        return *this;
    }
    
    template< std::size_t Order,
              class T = details:: normal_tag,
              typename std::enable_if_t< (Order > 0), std::nullptr_t> = nullptr,
              typename std::enable_if_t< std::is_same<T, details::immutable_tag>::value ||
                                         std::is_same<T, details::fleeting_tag>::value ||
                                         std::is_same<T, details::normal_tag>::value , std::nullptr_t > = nullptr >
    TATOElogger& add_header( char const * input_ph )
    {
        if( current_node_mh) {
            auto const & current_order = current_node_mh->get_value().order;
            int path = ( current_order > Order ) ? 1 : ( current_order < Order ? -1 : 0 );
            
            switch(path)
            {
                case -1:
                {
                    // std::cout << "lower_order" << std::endl;
                    current_node_mh = current_node_mh->add_child( header{
                        details::concatenate("----", input_ph, "----\n"),
                        status_value(T{}),
                        Order
                    } );
                    break;
                }
                case 0:
                {
                    // std::cout << "same_order" << std::endl;
                    current_node_mh =
                    current_node_mh->get_parent()->add_child( header{
                        details::concatenate("----", input_ph, "----\n"),
                        status_value(T{}),
                        Order
                    } );
                    break;
                }
                case 1:
                {
                    // std::cout << "higher_order" << std::endl;
                    // std::cout << input_ph << std::endl;
                    auto * parent_h = current_node_mh->find_parent( [](node_type const * node_h)
                                                                   { return node_h->get_value().order < Order;} );
                    // std::cout << parent_h->get_value().data << std::endl;
                    current_node_mh = parent_h->add_child( header{
                        details::concatenate("----", input_ph, "----\n"),
                        status_value(T{}),
                        Order
                    } );
                    break;
                }
            }
        }
        return *this;
    }
    
private:
    int status_value(details::fleeting_tag)      const { return -1;}
    int status_value(details::normal_tag)        const { return  0;}
    int status_value(details::immutable_tag)     const { return  1;}
    int status_value(details::non_removable_tag) const { return  2;}
    
public:
    template< class T = details:: normal_tag,
              typename std::enable_if_t< std::is_same<T, details::immutable_tag>::value ||
                                         std::is_same<T, details::fleeting_tag>::value ||
                                         std::is_same<T, details::normal_tag>::value , std::nullptr_t > = nullptr >
    TATOElogger& add_sub_header( char const * input_ph )
    {
        if( current_node_mh ) {
            current_node_mh = current_node_mh->add_child( header{
                details::concatenate("----", input_ph, "----\n"),
                status_value(T{}),
                current_node_mh->get_value().order +1
            } );
        }
        return *this;
    }
    
    template< class T >
    TATOElogger& operator<<(T && t_p)
    {
        if( current_node_mh ) {
            ostringstream buffer;
            buffer << t_p;
            current_node_mh->get_value().data += buffer.str();
        }
        return *this;
    }

    
    void freeze()
    {
        if ( current_node_mh ) {
            current_node_mh->get_value().status = 1;
        }
    }
    
    void clear()
    {
        root_mc.clear();
    }
    
    void output()
    {
        // std::cout << current_node_mh->get_value().data;
        
        auto process_immutable = [](node_type & node_p)
        {
            if(node_p.get_value().status == 1 && node_p.get_parent()->get_value().status != 1){
                node_p.walk_and_apply( [](node_type & node_p)
                                      { node_p.get_value().status = 1; } );
                node_p.reverse_walk_and_apply( [](node_type & node_p)
                                              { if(node_p.get_value().status == 0) {node_p.get_value().status = 2; } });
                node_p.lineage_walk( [](node_type & node_p)
                                    { node_p.get_value().status = 1; } );
            }
        };
        auto process_fleeting = [](node_type & node_p)
        {
            if(node_p.get_value().status == -1){
                node_p.get_value().data.clear();
                node_p.get_children().clear();
            }
        };
        auto output = [](node_type const & node_p)
        {
            if( node_p.get_value().status > 0){
                std::string output{node_p.get_value().data};
                if( node_p.get_value().status == 2 )
                {
                    output.erase( node_p.get_value().cursor );
                }
                std::cout << output;
            }
        };
        
        // auto nothing = [](node_type const & /*node_p*/){};
        
        for(auto& node_h : root_mc ){
            node_h->walk_and_apply( process_immutable );
            node_h->walk_and_apply( process_fleeting );
            node_h->walk_and_apply( output );
        }
        
    }
    
};


#endif
