//
//File      : arborescence.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//
#ifndef ARBORESCENCE_HPP
#define ARBORESCENCE_HPP



#include <vector>
#include <memory>
#include <iterator>
#include <type_traits>
#include <algorithm>



template<class Node>
class arborescence_handler;



template<class T>
struct node
{
    
private:
    T t_m;
    std::size_t depth_m;
    bool valid_m;
    std::vector< std::unique_ptr<node> > children_mc;
    node* parent_mh;
    
public:
    node(T&& t_p, node* parent_ph) :
        t_m{ std::move(t_p) },
        depth_m{ 0 },
        valid_m{ true },
        parent_mh{ parent_ph }
    {
        depth_m = ( parent_mh ?  parent_mh->depth_m + 1 :  1 );
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
    
    T const & get_value() const { return t_m; }
    std::size_t depth() const { return depth_m; }
    void mark_invalid() { valid_m = false; }
    bool is_childless() const { return children_mc.empty(); }
    bool is_valid() const { return valid_m; }
    
    
    std::vector< std::unique_ptr<node> >& get_children() { return children_mc; }
    std::vector< std::unique_ptr<node> > const & get_children() const { return children_mc; }
    
    std::vector<T> get_branch_values() const
    {
        std::vector<T> value_c;
        value_c.reserve( depth() );
        
        auto * node_h{ this };
        while( node_h->get_parent() ){
            value_c.push_back( node_h->get_value() );
            node_h = node_h->get_parent();
        }
        value_c.push_back( node_h->get_value() );
        
        std::reverse( value_c.begin(), value_c.end() );
        return value_c;
    }
    
    node const * const get_parent() const { return parent_mh; }
};



template< class Node >
class arborescence{
 

    friend class arborescence_handler<Node>;
    
    
    
private:
    std::vector< std::unique_ptr<Node> > roots_mc;
    std::vector< std::unique_ptr<Node> >& get_roots() { return roots_mc; }
    const std::vector< std::unique_ptr<Node> >& get_roots() const { return roots_mc; }
    
    std::unique_ptr< arborescence_handler< Node> > arborescence_handler_mhc;

   
    
public:
    arborescence() = default;
    
    template<class T>
    explicit arborescence( std::vector<T>&& roots_pc )
    {
        roots_mc.reserve( roots_pc.size() );
        for(auto& root : roots_pc ){
            roots_mc.emplace_back( new Node{ std::move(root), nullptr } );
        }
    }
    
    arborescence_handler<Node>& get_handler()
    {
        arborescence_handler_mhc.reset( new arborescence_handler<Node>{*this} );
        return *arborescence_handler_mhc;
    }
    
    template<class T>
    Node * add_root( T&& t_p )
    {
        roots_mc.emplace_back( new Node{ std::move(t_p), nullptr }  );
        return roots_mc.back().get();
    }
    
};


template<class T>
arborescence< node<T> > make_arborescence()
{
    return { };
}


template<class T>
arborescence< node<T> > make_arborescence( std::vector<T>&& t_pc)
{
    return arborescence< node<T> >{ std::move(t_pc) };
}




//--ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo--

template<class Node >
class arborescence_handler{
    friend arborescence_handler& arborescence<Node>::get_handler();
    
    class node_handler{
        
    private:
        Node* node_mh;
        
    public:
        Node& operator*(){ return *node_mh; }
        ~node_handler(){ node_mh = nullptr ; }
        
        node_handler(const node_handler&) = delete;
        node_handler& operator=(const node_handler&) = delete;
        
        node_handler(node_handler&& other_p) noexcept :
            node_mh{ std::move(other_p.node_mh) }
        {
            other_p.node_mh = nullptr;
        }
        node_handler& operator=(node_handler&& other_p) noexcept
        {
            node_mh = std::move( other_p.node_mh );
            other_p.node_mh = nullptr;
            return *this;
        }
        
        explicit node_handler( std::unique_ptr<Node>& node_ph ) : node_mh{ node_ph.get() } {}
    };
    
    
    struct iterator{
        using underlying_iterator = typename std::vector< node_handler >::iterator;
        
    private:
        underlying_iterator underlying_m;
        
    public:
        iterator( underlying_iterator iterator_p ) : underlying_m{ iterator_p } {}
        bool operator!=(const iterator& other_p) const { return underlying_m != other_p.underlying_m; }
        iterator& operator++() { ++underlying_m; return *this; }
        Node& operator*(){ return **underlying_m; }
    };
    
    
private:
    std::vector< node_handler > leaf_mc;
    
public:
    std::size_t size() { return leaf_mc.size(); }
    iterator begin() { return leaf_mc.begin(); }
    iterator end() { return leaf_mc.end(); }
    Node& operator[](std::size_t index_p) { return *leaf_mc[index_p]; }
    const Node& operator[](std::size_t index_p) const { return *leaf_mc[index_p]; }

    arborescence_handler(const arborescence_handler&) =delete;
    arborescence_handler& operator=(const arborescence_handler&) =delete;
    arborescence_handler(arborescence_handler&&) =delete;
    arborescence_handler& operator=(arborescence_handler&&) =delete;

    
private:
    explicit arborescence_handler( arborescence<Node>& arborescence_p ) :
        leaf_mc{ extract_all_leaves(arborescence_p) } {}
    std::vector< node_handler > extract_all_leaves( arborescence<Node>& arborescence_p  );
    std::vector< node_handler > extract_leaves_into( std::unique_ptr<Node>& node_ph, std::vector< node_handler >&& leaf_pc  );
};




template<class Node >
std::vector< typename arborescence_handler< Node >::node_handler > arborescence_handler< Node >::extract_all_leaves( arborescence<Node>& arborescence_p )
{
    auto& roots_c{ arborescence_p.get_roots() };
    
    using node_handler = typename arborescence_handler< Node >::node_handler;
    std::vector<node_handler> leaf_c;
    leaf_c.reserve( 10 * roots_c.size() );
    
    for( auto& node_h : roots_c ){
        leaf_c = extract_leaves_into( node_h, std::move(leaf_c) );
    }
    
    return leaf_c;
}




template<class Node >
std::vector< typename arborescence_handler< Node >::node_handler > arborescence_handler< Node >::extract_leaves_into( std::unique_ptr<Node>& node_ph, std::vector< typename arborescence_handler< Node >::node_handler >&& leaf_pc  )
{
    if( node_ph->is_childless() ) {
        if( node_ph->is_valid() ) { leaf_pc.emplace_back( node_ph ); }
    }
    else{
        auto& children_c{ node_ph->get_children() };
        for( auto& child_h : children_c ){
            leaf_pc = extract_leaves_into( child_h, std::move( leaf_pc ) );
        }
    }
    return std::move( leaf_pc );
}


#endif
