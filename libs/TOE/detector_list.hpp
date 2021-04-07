//
//File      : detector_list.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 17/12/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef DETECTOR_LIST_HPP
#define DETECTOR_LIST_HPP

#include <tuple>
namespace details{
    struct finished_tag{};
    
    template< class ... Ts> struct any_of : std::false_type {};
    template< class T> struct any_of<T> : T {};
    template< class T, class ... Ts>
    struct any_of<T, Ts...> : std::conditional< bool(T::value), T, any_of<Ts...>>::type {};
}

template<class ... Ts>
struct detector_list
{
    using tuple_type = std::tuple<Ts...>;
    
    constexpr detector_list(Ts&& ... t_p) : tuple_m{std::make_tuple(std::forward<Ts>(t_p)...)} {}
    
private:
    template<class T, std::size_t ... Indices>
    constexpr auto add_impl(T&& t_p, std::index_sequence<Indices...>)
                -> detector_list< Ts..., T >
    {
        return {std::get<Indices>(std::move(tuple_m))..., std::move(t_p) };
    }
    
public:
    template<class T>
    constexpr auto add(T t_p) &&
                -> detector_list< Ts..., T>
    {
        return add_impl(std::move(t_p), std::make_index_sequence< std::tuple_size<tuple_type>::value >() );
    }
    
    
    
private:
    template<std::size_t ... Indices>
    constexpr auto finish_impl(std::index_sequence<Indices...>)
                -> detector_list<details::finished_tag, Ts...>
    {
        return {std::get<Indices>(std::move(tuple_m))...};
    }
    
public:
    constexpr auto finish() &&
    {
        return finish_impl( std::make_index_sequence<std::tuple_size<tuple_type>::value>() );
    }
    
    
    
private:
    tuple_type tuple_m;
};



template<class ... Ts>
struct detector_list<details::finished_tag, Ts...>
{
    using tuple_type = std::tuple<Ts...>;
    
    constexpr detector_list(Ts&& ... t_p) : tuple_m{std::make_tuple(std::forward<Ts>(t_p)...)} { }
    
private:
    template< std::size_t Index, class F,
              typename std::enable_if_t< (Index < std::tuple_size<tuple_type>::value), std::nullptr_t > = nullptr >
    constexpr void apply_for_each_impl(F&& f_p) const
    {
        f_p( std::get<Index>(tuple_m) );
        apply_for_each_impl<Index+1>(std::forward<F>(f_p));
    }
    
    template< std::size_t Index, class F,
              typename std::enable_if_t< (Index >= std::tuple_size<tuple_type>::value), std::nullptr_t > = nullptr >
    constexpr void apply_for_each_impl(F&& /*f_p*/) const {}
    
    
    template< std::size_t Index, class F,
    typename std::enable_if_t< (Index < std::tuple_size<tuple_type>::value - 1 ), std::nullptr_t > = nullptr >
    constexpr void apply_except_last_impl(F&& f_p) const
    {
        f_p( std::get<Index>(tuple_m) );
        apply_except_last_impl<Index+1>(std::forward<F>(f_p));
    }
    
    template< std::size_t Index, class F,
    typename std::enable_if_t< (Index > std::tuple_size<tuple_type>::value - 2), std::nullptr_t > = nullptr >
    constexpr void apply_except_last_impl(F&& /*f_p*/) const {}
    
    
    template< std::size_t Index, class T,
    typename std::enable_if_t< std::is_same< T, typename std::tuple_element< Index, tuple_type>::type >::value ,
                                         std::nullptr_t > = nullptr >
    constexpr auto const& before_impl( T const & /*t_p*/ ) const
    {
        return std::get<Index - 1>(tuple_m);
    }
    
    template< std::size_t Index, class T,
    typename std::enable_if_t< !std::is_same< T, typename  std::tuple_element< Index, tuple_type>::type >::value ,
                                          std::nullptr_t > = nullptr >
    constexpr auto const& before_impl( T const & t_p ) const
    {
        static_assert(Index < std::tuple_size<tuple_type>::value - 1, "The element required is not a part of this list");
        return before_impl<Index+1>( t_p );
    }
    
public:
    template<class F>
    constexpr void apply_for_each(F&& f_p) const
    {
        apply_for_each_impl<0>( std::forward<F>(f_p) );
    }
    
    
    template<class F>
    constexpr void apply_except_last(F&& f_p) const
    {
        apply_except_last_impl<0>( std::forward<F>(f_p) );
    }
    
    
    constexpr auto const & first() const
    {
        return std::get<0>(tuple_m);
    }
    
    constexpr auto const & last() const
    {
        return std::get<std::tuple_size<tuple_type>::value -1 >(tuple_m);
    }
    
    
    template<class T>
    constexpr auto const & before( T const& t_p ) const
    {
        return before_impl<1>(t_p);
    }
    
    template<class T, class C, typename std::enable_if_t< details::any_of< std::is_same< Ts, T>... >::value , std::nullptr_t> = nullptr>
    constexpr void set_cuts( C cut_pc ) {
        std::get< T >( tuple_m ).set_cuts( std::move(cut_pc) );
    }
    
    template<class T, class C, typename std::enable_if_t< !details::any_of< std::is_same< Ts, T>... >::value, std::nullptr_t> = nullptr>
    constexpr void set_cuts( C /* */ ) {}
    
private:
    tuple_type tuple_m;
};



template<class ... Ts>
constexpr auto start_list(Ts... t_p) -> detector_list<Ts...> { return {std::move(t_p)... }; }


#endif /* DETECTOR_LIST_HPP */
