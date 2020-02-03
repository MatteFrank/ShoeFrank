//
//File      : TAGTOEArborescence.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 26/07/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//
#ifndef TAGTOE_ARBORESCENCE_HPP
#define TAGTOE_ARBORESCENCE_HPP



#include <vector>
#include <memory>
#include <iterator>
#include <type_traits>
#include <algorithm>

//debug
#include <iostream>
#include <string>


//maybe ?
namespace details{
    template< class InputIt, class OutputIt, class UnaryPredicate >
    OutputIt move_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred){
        return std::copy_if( std::make_move_iterator(first),
                             std::make_move_iterator(last  ),
                             d_first,
                             pred  );
    }

}

//this is the intended use
//std::copy_if( std::make_move_iterator(v1.begin()),
//             std::make_move_iterator(v1.end()  ),
//             std::back_inserter(v2),
//             [](M&& m_p){ return m_p.IsValid(); }  );





template< class T, class Node, class HandlingPolicy >
class TAGTOEArborescenceHandler;



namespace details{
    
    struct default_policy{
        using is_collapse_policy = std::false_type;
    };
    
    struct collapse_policy{
        using is_collapse_policy = std::true_type;
    };
    

    class parental_key{
        template<class Node>
        friend struct collapsable_handle_creator;
            
        template<class Node>
        friend struct collapsable_handle_deleter;
        
        parental_key() = default;
        parental_key(parental_key&&) = default;
        
        parental_key(const parental_key&) = delete;
        parental_key& operator=(const parental_key&) = delete;
        parental_key& operator=(parental_key&&) = delete;
    };
}




template<class T, class HandlingPolicy = details::default_policy >
class TAGTOEArborescence{
 

private:
    
    //SFINAE out invalid parameters ?
    template<class HandlingPolicy_, class Enabler = void>
    struct NodeEnhancer;
    
    template< class HandlingPolicy_>
    struct NodeEnhancer<HandlingPolicy_, std::enable_if_t< HandlingPolicy_::is_collapse_policy::value >>
    {
        bool IsCollapsable() const { return isCollapsable_m; }
        void MarkAsCollapsable(){ isCollapsable_m = true; }
    protected:
        bool isCollapsable_m{false};
    };
    
    template< class HandlingPolicy_ >
    struct NodeEnhancer<HandlingPolicy_, std::enable_if_t< !HandlingPolicy_::is_collapse_policy::value >>
    {
    };
    
    
    class Node : public NodeEnhancer<HandlingPolicy>{
        friend TAGTOEArborescence<T, HandlingPolicy>;
        
    private:
        T t_m;
        std::size_t depth_m;
        bool isValid_m;
        std::vector< std::unique_ptr<Node> > children_mc;
        Node* parent_mh;
        std::unique_ptr<Node> collapsableChild_mh;
        //probably at some point pointer to hit, or , some way to create a track ? -> in arborescence ?
        
    private:
        Node(T&& t_p, Node* parent_ph) :
            t_m{ std::move(t_p) },
            depth_m{ 0 },
            isValid_m{ true },
            parent_mh{ parent_ph }
        {
            parent_mh ? depth_m = parent_mh->depth_m + 1 : depth_m = 1 ;
            children_mc.reserve(5);
        }
    public:
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator=(Node&&) = delete;
        ~Node() = default;
        
        
    public:
        Node * AddChild(T&& t_p)
        // requires : isValid_m == true
        {
            children_mc.emplace_back( new Node{std::move(t_p) , this} );
            return children_mc.back().get();
        }
       
        const T& GetValue() const { return t_m; }
        std::size_t GetDepth() const { return depth_m; }
        void MarkAsInvalid() { isValid_m = false; }
        bool IsChildless() const { return children_mc.empty(); }
        bool IsValid() const { return isValid_m; }
   
        
        std::vector< std::unique_ptr<Node> >& GetChildren() { return children_mc; }
        const std::vector< std::unique_ptr<Node> >& GetChildren() const { return children_mc; }
        
        std::vector<T> GetBranchValues() const
        {
            std::vector<T> branchValues_c;
            branchValues_c.reserve( GetDepth() );
            
            auto node_h{ this };
            while( node_h->GetParent() ){
                branchValues_c.push_back( node_h->GetValue() );
                node_h = node_h->GetParent();
            }
            branchValues_c.push_back( node_h->GetValue() );
            
            std::reverse( branchValues_c.begin(), branchValues_c.end() );
            return branchValues_c;
        }//BranchHandler class maybe in order not to copy ? that will do for now
        
    private:
        Node* GetParent() const { return parent_mh; }
    public:
        Node* GetParent(details::parental_key) const { return parent_mh; }

        
    private:
        //for debug purpose, to be removed
        void OutputBranches(std::string branch_p) const
        {
            branch_p += " " + std::to_string(this->GetValue());
            if( IsChildless() ){ std::cout << branch_p << '\n'; }
            else{
                for(const auto& child_h : children_mc ){
                    child_h->OutputBranches( branch_p );
                }
            }
        }
    };
    
    
    template< class T_, class Node_, class HandlingPolicy_ >
    friend class TAGTOEArborescenceHandler;
    
public:
    using node = Node;
    
    
private:
    std::vector< std::unique_ptr<Node> > roots_mc;
    std::vector< std::unique_ptr<Node> >& GetRoots() { return roots_mc; }
    const std::vector< std::unique_ptr<Node> >& GetRoots() const { return roots_mc; }
    
    std::unique_ptr< TAGTOEArborescenceHandler<T, Node, HandlingPolicy> > arborescenceHandler_mhc;

    //for debug, to be removed
public:
    void WalkArborescence()
    {
        auto& roots_c{ GetRoots() };
        for(const auto& node_h : roots_c ){
            node_h->OutputBranches(std::string{});
        }
    }
    
    
   
    
public:
    TAGTOEArborescence() = default;
    
    explicit TAGTOEArborescence( std::vector<T>&& t_pc )
    {
        roots_mc.reserve( t_pc.size() );
        for(auto& t : t_pc ){
            roots_mc.emplace_back( new Node{ std::move(t), nullptr } );
        }
    }
    
    TAGTOEArborescenceHandler<T, Node, HandlingPolicy>& GetHandler()
    {
      //  std::cout << "Old handler deletion...\n";
        //arborescenceHandler_mhc.get_deleter()( arborescenceHandler_mhc.get() );
        arborescenceHandler_mhc.reset(nullptr);
     //   std::cout << "Old handler deleted\n";
        
    //    if(arborescenceHandler_mhc.get()){std::cout << "Old handler is not empty\n";}
        
     //   std::cout << "Arborescence is :\n";
     //   this->WalkArborescence();
        
        auto* newHandler_hc{ new TAGTOEArborescenceHandler<T, Node, HandlingPolicy>{*this} };
     //   if(arborescenceHandler_mhc.get()){std::cout << "Old handler is empty\n";}
      //  std::cout << "Reseting unique_ptr\n";
        arborescenceHandler_mhc.reset( newHandler_hc );
      //  std::cout << "New handler holds: ";
      //  for(auto& leaf : *arborescenceHandler_mhc ){std::cout << leaf.GetValue() << " "; }
       // std::cout << '\n';
        return *arborescenceHandler_mhc;
    }
    
    
    Node * add_root( T&& t_p )
    {
        roots_mc.emplace_back( new Node{ std::move(t_p), nullptr }  );
        return roots_mc.back().get();
    }
    
    
    
};


template<class T>
TAGTOEArborescence<T> make_arborescence()
{
    return { };
}


template<class T>
TAGTOEArborescence<T> make_arborescence( std::vector<T>&& t_pc)
{
    return TAGTOEArborescence<T>{ std::move(t_pc) };
}


template<class T>
TAGTOEArborescence<T, details::collapse_policy> make_collapsable_arborescence( std::vector<T>&& t_pc)
{
    return TAGTOEArborescence<T, details::collapse_policy>{ std::move(t_pc) };
}
//--ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo--

namespace details {

    
    template<class Node>
    struct default_handle_deleter{
        void operator()(Node*& node_ph){ node_ph = nullptr; }
    };
    
    
    template<class Node>
    struct default_handle_creator{
        Node* operator()(std::unique_ptr<Node>& node_ph){ return node_ph.get(); }
    };

    
    
    template<class Node>
    struct collapsable_handle_creator{
        Node* operator()(std::unique_ptr<Node>& node_ph)
        {
            Node* node_h{ nullptr };
            //std::cout << "handle_creator: " << node_ph->GetValue() << '\n';
            if( node_ph->IsCollapsable() ){
                node_h = node_ph.release();
                node_h->GetParent(details::parental_key{})->GetChildren().pop_back();
            }
            else{
                node_h = node_ph.get();
            }
            return node_h;
        }
    };
    
    template<class Node>
    struct collapsable_handle_deleter{
        void operator()(Node*& node_ph){
            //requires : collapsable_node == GetChildren.back()
            //std::cout << "handle_deleter: \n";
            if(node_ph){
                //std::cout << "value: " << node_ph->GetValue() << '\n';
                if( node_ph->IsCollapsable()  ){
                    //std::cout << "+ collapsable\n";
                    if( !node_ph->IsChildless() ){
                        //std::cout << "+ notchildless\n";
                        auto& targetChildren_c = node_ph->GetParent(details::parental_key{})->GetChildren();
                        auto& sourceChildren_c = node_ph->GetChildren();
                        targetChildren_c.insert( targetChildren_c.end(),
                                                std::make_move_iterator(sourceChildren_c.begin()) ,
                                                std::make_move_iterator(sourceChildren_c.end())      );
                    }
                    else{
                        node_ph->GetParent(details::parental_key{})->MarkAsInvalid();
                    }
                    
                    delete node_ph;
                }
                else{
                    auto value{ node_ph->GetValue() };
                    node_ph->AddChild( std::move(value) );
                    node_ph->GetChildren().back()->MarkAsCollapsable();
                }
                node_ph = nullptr;
            }
        }
    };
}



template< class Node,
          class Creator = details::default_handle_creator<Node>,
          class Deleter = details::default_handle_deleter<Node>  >
class NodeHandler{
    
   // template<class T, class Node_ = Node, class HandlingPolicy>
//    template<class T, class HandlingPolicy>
//    friend void TAGTOEArborescenceHandler<T, Node, HandlingPolicy>::ExtractLeaves( std::unique_ptr<Node>& node_ph ) TAGTOEArborescenceHandler;
    
private:
    Node* underlyingNode_mh;
    
public:
    Node& operator*(){ return *underlyingNode_mh; }
    ~NodeHandler(){ Deleter{}(underlyingNode_mh); }
    
    NodeHandler(const NodeHandler&) = delete;
    NodeHandler& operator=(const NodeHandler&) = delete;

    NodeHandler(NodeHandler&& other_p) noexcept :
        underlyingNode_mh{ std::move(other_p.underlyingNode_mh) }
    {
        other_p.underlyingNode_mh = nullptr;
  //      std::cout << "NodeHandler(NodeHandler&&): "<< underlyingNode_mh->GetValue() <<'\n';
    }
    NodeHandler& operator=(NodeHandler&& other_p) noexcept
    {
        //no self-assignement possible, so no check
        underlyingNode_mh = std::move(other_p.underlyingNode_mh);
        other_p.underlyingNode_mh = nullptr;
   //     std::cout << "NodeHandler = NodeHandler&&: "<< underlyingNode_mh->GetValue() <<'\n';
        return *this;
    }
    //NodeHandler() : underlyingNode_mh{nullptr} {}
    

        //in the best of worlds, private and accessible only by extract ? -> key ?
    explicit NodeHandler(std::unique_ptr<Node>& node_ph) : underlyingNode_mh{ Creator{}(node_ph) } {}
};






//--ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo----ooo000OOOO000ooo--

template<class T, class Node, class HandlingPolicy = details::default_policy >
class TAGTOEArborescenceHandler{
    //might need a killer
    
    friend TAGTOEArborescenceHandler& TAGTOEArborescence<T, HandlingPolicy>::GetHandler();
    
    using NodeHandler_ = typename std::conditional<
                                                    HandlingPolicy::is_collapse_policy::value,
                                                    NodeHandler<
                                                                Node,
                                                                details::collapsable_handle_creator<Node>,
                                                                details::collapsable_handle_deleter<Node>
                                                               >,
                                                    NodeHandler<Node>
                                                   >::type;
    
    class iterator{
        using underlying_iterator = typename std::vector< NodeHandler_ >::iterator;
        
    private:
        underlying_iterator underlying_m;
        
    public:
        iterator( underlying_iterator iterator_p ) : underlying_m{ iterator_p } {}
        bool operator!=(const iterator& other_p) const { return underlying_m!=other_p.underlying_m; }
        iterator& operator++() { ++underlying_m; return *this; }
        Node& operator*(){ return **underlying_m; }
    };
    
    
   
    
private:
    std::vector< NodeHandler_ > leaves_mc;
    
public:
    std::size_t size() { return leaves_mc.size(); }
    iterator begin() { return leaves_mc.begin(); }
    iterator end() { return leaves_mc.end(); }
    Node& operator[](std::size_t index_p) { return *leaves_mc[index_p]; }
    const Node& operator[](std::size_t index_p) const { return *leaves_mc[index_p]; }

    TAGTOEArborescenceHandler(const TAGTOEArborescenceHandler&) =delete;
    TAGTOEArborescenceHandler& operator=(const TAGTOEArborescenceHandler&) =delete;
    TAGTOEArborescenceHandler(TAGTOEArborescenceHandler&&) =delete;
    TAGTOEArborescenceHandler& operator=(TAGTOEArborescenceHandler&&) =delete;
//    ~TAGTOEArborescenceHandler(){
//        std::cout << "ArborescenceHandler deletion\n";
//        std::cout << "values: ";
//        for(auto& node_h : leaves_mc){
//            std::cout << (*node_h).GetValue() << " ";
//        }
//        std::cout << '\n';
//    }
    //might need to custom it
    
private:
    explicit TAGTOEArborescenceHandler(TAGTOEArborescence<T, HandlingPolicy>& arborescence_p );
    void ExtractLeaves( std::unique_ptr<Node>& startNode_ph );
};




template<class T, class Node, class HandlingPolicy >
TAGTOEArborescenceHandler<T, Node, HandlingPolicy>::TAGTOEArborescenceHandler(TAGTOEArborescence<T, HandlingPolicy>& arborescence_p )
{
    auto& roots_c{ arborescence_p.GetRoots() };
    leaves_mc.reserve( 10 * roots_c.size() );
 //   std::cout << "Creation of handler\n";
    for(auto& node_h : roots_c ){
    //    std::cout << "Value: " << node_h->GetValue() << '\n';
        ExtractLeaves( node_h );
  //      std::cout << "done\n";
    }
}




template<class T, class Node, class HandlingPolicy >
void TAGTOEArborescenceHandler<T, Node, HandlingPolicy>::ExtractLeaves( std::unique_ptr<Node>& node_ph )
{
    if( node_ph->IsChildless() ) {
        if( node_ph->IsValid() ) { leaves_mc.emplace_back( node_ph ); }
    }
    else{
        auto& children_c{ node_ph->GetChildren() };
        for( auto& child_h : children_c ){
            ExtractLeaves( child_h );
        }
    }
}


//maybe define node outside of arborescence ? templated on nodes then ?

#endif
