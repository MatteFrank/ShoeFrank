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
}

template<class ... Ts>
struct detector_list
{
    using tuple_type = std::tuple<Ts...>;
    
    detector_list(Ts&& ... t_p) : tuple_m{std::make_tuple(std::forward<Ts>(t_p)...)} {}
    
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
    
    detector_list(Ts&& ... t_p) : tuple_m{std::make_tuple(std::forward<Ts>(t_p)...)} { }
    
private:
    template< std::size_t Index, class F,
              typename std::enable_if_t< (Index < std::tuple_size<tuple_type>::value), std::nullptr_t > = nullptr >
    constexpr void apply_for_each_impl(F&& f_p)
    {
        f_p( std::get<Index>(tuple_m) );
        apply_for_each_impl<Index+1>(std::forward<F>(f_p));
    }
    
    template< std::size_t Index, class F,
              typename std::enable_if_t< (Index >= std::tuple_size<tuple_type>::value), std::nullptr_t > = nullptr >
    constexpr void apply_for_each_impl(F&& /*f_p*/){}
    
public:
    template<class F>
    constexpr void apply_for_each(F&& f_p)
    {
        apply_for_each_impl<0>( std::forward<F>(f_p) );
    }
    
private:
    const tuple_type tuple_m;
};



template<class ... Ts>
auto start_list(Ts... t_p) -> detector_list<Ts...> { return {std::move(t_p)... }; }


#endif /* DETECTOR_LIST_HPP */
