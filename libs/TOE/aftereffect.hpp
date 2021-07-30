/*
 \author A. Sécher
 */

//
//File      : aftereffect.hppp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


#ifndef _AFTEREFFECT_HPP
#define _AFTEREFFECT_HPP
    
#include <tuple>

namespace aftereffect {
    template<class T>
    struct optional{
        T t_m;
        T* t_mh;
        
        template< class T_ = T,
        typename std::enable_if_t< std::is_default_constructible<T_>::value, std::nullptr_t > = nullptr >
        explicit optional( T t_p  ) : t_m{ std::move(t_p) }, t_mh{ &t_m } {}
        
        template< class T_ = T,
        typename std::enable_if_t< std::is_default_constructible<T_>::value, std::nullptr_t > = nullptr >
        optional() : t_m{}, t_mh{nullptr} {}
        
        optional(optional const& rhs_p){
            if(rhs_p.has_value()){
                t_m = rhs_p.t_m;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
        }
        optional(optional&& rhs_p){
            if(rhs_p.has_value()){
                t_m = std::move(rhs_p.t_m);
                rhs_p.t_mh = nullptr;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
        }
        optional& operator=(optional const& rhs_p){
            if(rhs_p.has_value()){
                t_m = rhs_p.t_m;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
            return *this;
        }
        optional& operator=(optional&& rhs_p){
            if(rhs_p.has_value()){
                t_m = std::move( rhs_p.t_m );
                rhs_p.t_mh = nullptr;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
            return *this;
        }
        
        T const& value() const { return t_m; }
        T& value() { return t_m; }
        
        bool has_value() const { return t_mh != nullptr; }
    };

    
    
    template<class ... Ts>
    struct pack{};
    
    
    template<class F> struct callable_signature : callable_signature<decltype(&F::operator())> {};
    
    template<class T, class TReturn, class ...TArgs>
    struct callable_signature<TReturn(T::*)(TArgs...) const> {
        using return_type = TReturn;
        using argument_list = pack<TArgs...>;
    };
    
    
    template<class P, class O>
    struct aftereffect {
        P const predicate;
        O const outcome;
        
        using return_type = typename callable_signature<O>::return_type;
        
        template< class P_ = P,
                  class ArgumentCheck = std::enable_if_t< std::is_same< typename callable_signature<P_>::argument_list,
                                                                        typename callable_signature<O>::argument_list >::value  > >
        constexpr aftereffect(P predicate_p, O outcome_p) : predicate{ std::move(predicate_p) }, outcome{std::move(outcome_p)} {}
        
        template< class ... TArgs,
                  class O_ = O,
                  typename std::enable_if_t< !std::is_same< typename callable_signature<O_>::return_type, void>::value , std::nullptr_t> = nullptr >
        constexpr optional<return_type> operator()(TArgs&& ... args) const {
            if( predicate( std::forward<TArgs>(args)... ) ){ return optional<return_type>{ outcome( std::forward<TArgs>(args)... ) }; }
            return optional<return_type>{};
        }
        
        template< class ... TArgs,
                  class O_ = O,
                  typename std::enable_if_t< std::is_same< typename callable_signature<O_>::return_type, void>::value , std::nullptr_t> = nullptr >
        constexpr void operator()(TArgs&& ... args) const {
            if( predicate( std::forward<TArgs>(args)... ) ){ outcome( std::forward<TArgs>(args)... ); }
        }
        
    };
    
    
    template<class P, class O>
    constexpr auto make_aftereffect(P predicate_p, O outcome_p) -> aftereffect<P, O> {
        return {std::move(predicate_p), std::move(outcome_p)};
    }
    
    
    template<class Value, class ... As> struct first_of_impl{};
    
    template<class Value, class A, class ... As>
    struct first_of_impl<Value, A , As...>{
        static constexpr auto apply( Value const& value, A const& a, As const& ... as ) {
            auto current_result = a(value);
            if( current_result.has_value() ){  return current_result; }
            else{ return first_of_impl<Value, As...>::apply( value, as...  ); }
        }
    };
    
    template<class Value, class A>
    struct first_of_impl<Value, A >{
        static constexpr auto apply( Value const& value, A const& a ) { return a(value); }
    };

    template<class Value, class ... As>
    constexpr auto first_of( Value const& value, As const& ... as  ) {
        return first_of_impl<Value, As...>::apply( value, as... );
    }
    
    //doesn't work for empty containers
    template<class Iterator, class ... As>
    constexpr auto first_of( Iterator first, Iterator last, As const& ... as  ) {
        for (; first != last-1; ++first) {
            auto result_o = first_of( *first, as... );
            if( result_o.has_value() ){ return result_o; }
        }
        return first_of( *first, as...  );
    }
    
    template<class Value, class ... Fs> struct apply_impl{};
    template<class Value, class F, class ... Fs>
    struct apply_impl< Value, F, Fs...>{
        static constexpr void apply( Value const& value_p, F const f_p, Fs const& ...fs_p ){
            f_p(value_p);
            apply_impl<Value, Fs...>::apply(value_p, fs_p...);
        }
    };
    
    template<class Value, class F>
    struct apply_impl< Value, F>{
        static constexpr void apply( Value const& value_p, F const f_p ){ f_p(value_p); }
    };
    
    template<class Value>
    struct apply_impl< Value>{
        static constexpr void apply( Value const&  ){ ; }
    };

    template<class Value, class ... Fs>
    constexpr void apply( Value const& value_p, Fs const&  ... fs ) {
        apply_impl<Value, Fs...>::apply(value_p, fs...);
    }
    
    
    
    template<class ...As>
    struct aftereffect_list{
        using tuple_type = typename std::tuple<As...>;
        
    private:
        tuple_type aftereffect_mc;
        
    public:
        aftereffect_list( As ... as_p ) : aftereffect_mc{ std::move(as_p)... } {}
        
        template< class Value >
        void evaluate( Value const& value_p) {
            evaluate_impl( value_p, std::make_index_sequence< sizeof...(As) >{} );
        }
        
    private:
        template<class Value, std::size_t ...Indices>
        void evaluate_impl( Value const& value_p, std::index_sequence<Indices...> ) {
            apply( value_p, std::get< Indices >(aftereffect_mc)...);
        }
        
    };
    
    template<class ...As>
    constexpr auto make_aftereffect_list( As&& ... as_p ) ->aftereffect_list<As...> {
        return {std::forward<As>(as_p)...};
    }
    
} // namespace aftereffect


#endif
