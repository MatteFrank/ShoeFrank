//
//File      : expr.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 01/04/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef expr_hpp
#define expr_hpp

#include <array>
#include <tuple>

#include <cmath>

#include "utility_types.hpp"
#include "matrix_new.hpp"

template<std::size_t NRows, std::size_t NCols>
class matrix;

namespace expr {
    
    template<class Derived>
    struct base_expr
    {
        constexpr Derived const& derived() const { return static_cast<Derived const&>( *this); }
        constexpr Derived && derived() { return static_cast<Derived&&>( *this); }
        constexpr auto operator()() const { return derived()(); }
        
        template<class Output>
        constexpr operator Output() const { return derived()(); }
    };
    
    template<std::size_t NRows, std::size_t NCols, class ... Exprs>
    struct matrix_expr : base_expr< matrix_expr<NRows, NCols, Exprs ...> >
    {
        constexpr explicit matrix_expr( std::tuple<Exprs...> expr_pc ) : expr_mc{std::move(expr_pc)} {}
        constexpr matrix<NRows, NCols> operator()() const
        {
            return compute_matrix( std::make_index_sequence<NRows * NCols>() );
        }
        constexpr std::tuple<Exprs...> const& get_expressions() const { return expr_mc; }
        
    private:
        template<std::size_t ... Indices>
        constexpr matrix<NRows, NCols> compute_matrix( std::index_sequence<Indices...> ) const
        {
            return { std::get<Indices>( expr_mc )()... };
        }
        
        std::tuple<Exprs...> expr_mc;
    };
    
    template<std::size_t NRows, std::size_t NCols, class ... Exprs>
    constexpr auto make_matrix_expr( Exprs&& ... exprs )
    {
        return matrix_expr<NRows, NCols, Exprs ...>{std::make_tuple(exprs...)};
    }
    
    template< std::size_t NRows, std::size_t NCols,
    class Filler, std::size_t... Indices>
    constexpr auto make_matrix_expr_impl( Filler f_p,
                                         std::index_sequence<Indices...>)
    {
        // puts(__PRETTY_FUNCTION__);
        return make_matrix_expr<NRows, NCols>( f_p.template call<Indices>()... );
    }
    
    template< std::size_t IRow, std::size_t ICol,
    std::size_t NRows, std::size_t NCols, class ... Exprs>
    constexpr auto get( matrix_expr<NRows, NCols, Exprs...> const&  m_p )
    {
        return std::get< IRow * NCols + ICol >( m_p.get_expressions() );
    }
    
    
    template<std::size_t NRows, std::size_t NCols, class ... Exprs>
    constexpr matrix<NRows, NCols> compute( matrix_expr<NRows, NCols, Exprs ...>&& m_expr_p )
    {
        return m_expr_p();
    }
    
    
    
    template<class Operation, class A>
    struct unary_expr : base_expr< unary_expr<Operation, A> >
    {
        explicit constexpr unary_expr(A a_p) : a{std::move(a_p)} {}
        constexpr auto operator()() const { return op(a()); }
        
    private:
        A a;
        Operation op;
    };
    
    template<class Operation, class A, class B>
    struct binary_expr : base_expr< binary_expr<Operation, A, B> >
    {
        constexpr binary_expr(A a_p, B b_p) : a{std::move(a_p)}, b{std::move(b_p)} {}
        constexpr auto operator()() const { return op(a(), b()); }
        
        A a;
        B b;
        Operation op;
    };
    
    template< class Operation, class ... Ts >
    struct Nary_expr : base_expr< Nary_expr< Operation, Ts...> >
    {
        constexpr Nary_expr(Ts... ts_p) : ts_mc{ std::make_tuple(std::move(ts_p) ...) } { }
        constexpr auto operator()() const { return apply_operation( std::make_index_sequence<sizeof...(Ts)>() ); }
        
    private:
        template<std::size_t ... Indices>
        constexpr auto apply_operation( std::index_sequence<Indices...> ) const
        { return op( std::get<Indices>(ts_mc)()... ); }
        
        std::tuple<Ts...> ts_mc;
        Operation op;
    };
    
    template< class Operation, class ... Ts>
    constexpr auto make_Nary_expr(Ts... ts_p)
    -> Nary_expr<Operation, Ts...>
    {
        return { std::move(ts_p)... };
    }
    
    template<class Value>
    struct terminal_expr : base_expr< terminal_expr<Value> >
    {
        explicit constexpr terminal_expr( Value v_p ) : v{v_p} {}
        constexpr auto operator()() const {return v;}
        
    private:
        Value v;
    };
    
    template<class Value>
    constexpr auto make_terminal_expr(Value const& v_p)
    {
        return terminal_expr<Value>{ v_p };
    }
    
    
    
    
    template<class ... Ts> struct add_impl{};
    template<class T, class ... Ts> struct add_impl<T, Ts...> {
        constexpr static auto apply(T const& t_p, Ts const& ... ts_p){ return t_p + add_impl<Ts...>::apply(ts_p...); }
    };
    template<class T, class U> struct add_impl<T, U> { constexpr static auto apply(T const& t_p, U const& u_p){  return t_p + u_p; } };
    template<class T> struct add_impl<T> { constexpr static auto apply(T const& t_p){  return t_p ; } };
    
    template<class ...Ts>
    constexpr auto add(Ts const& ... ts_p){ return add_impl<Ts...>::apply(ts_p...); }
    
    struct addition_operation
    {
        template<class ... Ts>
        constexpr auto operator()(Ts const& ... ts_p) const { return add(ts_p...);}
    };
    
    struct substraction_operation
    {
        template<class T, class U>
        constexpr auto operator()(T const& t_p, U const&  u_p) const { return t_p - u_p;}
    };
    
   
    
    
    template<class ... Ts> struct multiply{};
    template<class T, class ... Ts> struct multiply<T, Ts...> { constexpr static auto apply(T const& t_p, Ts const& ... ts_p)
        { return t_p * multiply<Ts...>::apply(ts_p...); } };
    template<class T, class U> struct multiply<T, U> {
        constexpr static auto apply(T const& t_p, U const& u_p){ return t_p * u_p; }
    };
    
    
    struct multiplication_operation
    {
        template<class ... Ts>
        constexpr auto operator()(Ts const& ... ts_p) const {return multiply<Ts...>::apply(ts_p...);}
    };
    
    
    
    template< class Expr1, class Expr2>
    constexpr auto operator*(base_expr<Expr1> const& expr1, base_expr<Expr2> const& expr2)
    {
        return binary_expr<multiplication_operation, Expr1, Expr2>{expr1.derived(), expr2.derived()};
    }

    template< class Expr1, class Expr2>
    constexpr auto operator+(base_expr<Expr1> const& expr1, base_expr<Expr2> const& expr2)
    {
        return binary_expr<addition_operation, Expr1, Expr2>{expr1.derived(), expr2.derived()};
    }
    
    template< class Expr1, class Expr2>
    constexpr auto operator-(base_expr<Expr1> const& expr1, base_expr<Expr2> const& expr2)
    {
        return binary_expr<substraction_operation, Expr1, Expr2>{expr1.derived(), expr2.derived()};
    }
    
    
    
    template< std::size_t NRows, std::size_t NCols, class Expr, class MExpr>
    struct mixed_multiplication_filler
    {
        //matrices as matrix_expr directly ? like a terminal matrix_expr ?
        constexpr mixed_multiplication_filler(  base_expr<Expr> && expr_p, base_expr<MExpr> && m_expr_p ) : expr{expr_p.derived()}, m_expr{m_expr_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return expr * get<Index/NCols, Index%NCols>(m_expr);
        }
        
    private:
        Expr const& expr;
        MExpr const& m_expr;
    };
    
    
    
    template< std::size_t NRows, std::size_t N, std::size_t NCols, class MExpr1, class MExpr2>
    struct matrix_multiplication_filler
    {
        //matrices as matrix_expr directly ? like a terminal matrix_expr ?
        constexpr matrix_multiplication_filler(  base_expr<MExpr1> && m1_p, base_expr<MExpr2> && m2_p ) : m1{m1_p.derived()}, m2{m2_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return form_higher_expr<Index/NCols, Index%NCols>( std::make_index_sequence<N>() );
        }
        
    private:
        template<std::size_t IRow, std::size_t ICol, std::size_t ... Indices>
        constexpr auto form_higher_expr( std::index_sequence<Indices...>) const
        {
            //return add( form_lower_expr( row_index_p, Indices, column_index_p)... );
            // puts(__PRETTY_FUNCTION__);
            return make_Nary_expr<addition_operation>( form_lower_expr<IRow, Indices, ICol>()... );
        }
        
        template<std::size_t IRow, std::size_t IRunning, std::size_t ICol>
        constexpr auto form_lower_expr() const
        {
            // puts(__PRETTY_FUNCTION__);
            return get<IRow, IRunning>(m1) * get<IRunning, ICol>(m2);
        }
        
        MExpr1 const& m1;
        MExpr2 const& m2;
    };
    
    template<std::size_t NRows, std::size_t NCols>
    struct terminal_filler{
        constexpr explicit terminal_filler(  matrix<NRows, NCols> const & m_p ) : m{m_p} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return make_terminal_expr( m(Index/NCols, Index % NCols) );
        }
        
    private:
        matrix<NRows, NCols> const& m;
    };
    
    template<std::size_t NRows, std::size_t NCols>
    struct empty_terminal_filler{
        template<std::size_t Index>
        constexpr auto call() const
        {
            return make_terminal_expr( 0 );
        }
    };
    

    
    
    template< std::size_t NRows, std::size_t NCols, class MExpr1, class MExpr2>
    struct addition_filler
    {
        constexpr addition_filler( base_expr<MExpr1> && m1_p, base_expr<MExpr2> && m2_p ) : m1{m1_p.derived()}, m2{m2_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return get<Index/NCols, Index%NCols>(m1) + get<Index/NCols, Index%NCols>(m2);
        }
        
    private:
        MExpr1 const& m1;
        MExpr2 const& m2;
    };
    
    
    template< std::size_t NRows, std::size_t NCols, class MExpr1, class MExpr2>
    struct substraction_filler
    {
        constexpr substraction_filler( base_expr<MExpr1> && m1_p, base_expr<MExpr2> && m2_p ) : m1{m1_p.derived()}, m2{m2_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return get<Index/NCols, Index%NCols>(m1) - get<Index/NCols, Index%NCols>(m2);
        }
        
    private:
        MExpr1 const& m1;
        MExpr2 const& m2;
    };

    
    template< std::size_t NRows, std::size_t NCols, class MExpr >
    struct transpose_filler
    {
        constexpr transpose_filler( base_expr<MExpr> && m_p ) : m{m_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return get<Index%NRows, Index/NRows>(m);
        }
        
    private:
        MExpr const& m;
    };
    

    
    
    template< std::size_t IRow, class MExpr >
    struct row_filler
    {
        constexpr row_filler( base_expr<MExpr> && m_p ) : m{m_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return get<IRow, Index>(m);
        }
        
    private:
        MExpr const& m;
    };
    
  
    
    
    template< std::size_t ICol, class MExpr >
    struct column_filler
    {
        constexpr column_filler( base_expr<MExpr> && m_p ) : m{m_p.derived()} {}
        template<std::size_t Index>
        constexpr auto call() const
        {
            return get<Index, ICol>(m);
        }
        
    private:
        MExpr const& m;
    };

} //namespace expr


//-------------------------------ADDITION----------------------------


template< std::size_t NRows, std::size_t NCols >
constexpr auto operator+( matrix<NRows, NCols> const& m1_p,
                          matrix<NRows, NCols> const& m2_p )
{
    // puts(__PRETTY_FUNCTION__);
    auto&& m_expr1 = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m1_p), std::make_index_sequence<NRows * NCols>() );
    auto&& m_expr2 = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m2_p), std::make_index_sequence<NRows * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr1)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr2)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::addition_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr1), std::move(m_expr2)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t NCols, class... Exprs >
constexpr auto operator+( expr::matrix_expr<NRows, NCols, Exprs...> && m_expr_p,
                          matrix<NRows, NCols> const& m_p )
{
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr_p)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::addition_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr_p), std::move(m_expr)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                    );
}


template< std::size_t NRows, std::size_t NCols, class... Exprs>
constexpr auto operator+( matrix<NRows, NCols> const& m_p,
                          expr::matrix_expr<NRows, NCols, Exprs...> && m_expr_p )
{
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                    expr::addition_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr), std::move(m_expr_p)} ,
                                    std::make_index_sequence<NRows * NCols>()
                                               );
}


template< std::size_t NRows, std::size_t NCols, class ... Exprs1, class ... Exprs2 >
constexpr auto operator+( expr::matrix_expr<NRows, NCols, Exprs1...> && m_expr1_p,
                          expr::matrix_expr<NRows, NCols, Exprs2...> && m_expr2_p )
{
    using MExpr1 = typename std::decay_t<decltype(m_expr1_p)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr2_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::addition_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr1_p), std::move(m_expr2_p)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


//-------------------------------SUBSTRACTION----------------------------


template< std::size_t NRows, std::size_t NCols >
constexpr auto operator-( matrix<NRows, NCols> const& m1_p,
                          matrix<NRows, NCols> const& m2_p )
{
    // puts(__PRETTY_FUNCTION__);
    auto&& m_expr1 = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m1_p), std::make_index_sequence<NRows * NCols>() );
    auto&& m_expr2 = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m2_p), std::make_index_sequence<NRows * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr1)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr2)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::substraction_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr1), std::move(m_expr2)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t NCols, class... Exprs >
constexpr auto operator-( expr::matrix_expr<NRows, NCols, Exprs...> && m_expr_p,
                         matrix<NRows, NCols> const& m_p )
{
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr_p)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::substraction_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr_p), std::move(m_expr)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t NCols, class... Exprs>
constexpr auto operator-( matrix<NRows, NCols> const& m_p,
                         expr::matrix_expr<NRows, NCols, Exprs...> && m_expr_p )
{
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::substraction_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr), std::move(m_expr_p)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t NCols, class ... Exprs1, class ... Exprs2 >
constexpr auto operator-( expr::matrix_expr<NRows, NCols, Exprs1...> && m_expr1_p,
                         expr::matrix_expr<NRows, NCols, Exprs2...> && m_expr2_p )
{
    using MExpr1 = typename std::decay_t<decltype(m_expr1_p)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr2_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::substraction_filler<NRows, NCols, MExpr1, MExpr2>{ std::move(m_expr1_p), std::move(m_expr2_p)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}

//-------------------------------------MULTIPLICATION-------------------------------





template< std::size_t NRows, std::size_t NCols, class T, class Enabler = std::enable_if_t<std::is_arithmetic<T>::value> >
constexpr auto operator*( T const& t_p,
                          matrix<NRows, NCols> const& m_p )
{
    // puts(__PRETTY_FUNCTION__);
    auto&& expr = expr::terminal_expr<T>( t_p );
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    using Expr = typename std::decay_t<decltype(expr)>;
    using MExpr = typename std::decay_t<decltype(m_expr)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::mixed_multiplication_filler<NRows, NCols, Expr, MExpr>{ std::move(expr), std::move(m_expr)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t NCols, class T, class Enabler = std::enable_if_t<std::is_arithmetic<T>::value> >
constexpr auto operator*( matrix<NRows, NCols> const& m_p,
                          T const& t_p)
{
    // puts(__PRETTY_FUNCTION__);
    auto&& expr = expr::terminal_expr<T>( t_p );
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    using Expr = typename std::decay_t<decltype(expr)>;
    using MExpr = typename std::decay_t<decltype(m_expr)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                                     expr::mixed_multiplication_filler<NRows, NCols, Expr, MExpr>{ std::move(expr), std::move(m_expr)} ,
                                                     std::make_index_sequence<NRows * NCols>()
                                                     );
}



template< std::size_t NRows, std::size_t NCols, class... Exprs, class T, class Enabler = std::enable_if_t<std::is_arithmetic<T>::value> >
constexpr auto operator*( T const& t_p,
                          expr::matrix_expr<NRows, NCols, Exprs...> && m_expr_p )
{
    // puts(__PRETTY_FUNCTION__);
    auto&& expr = expr::terminal_expr<T>( t_p );
    using Expr = typename std::decay_t<decltype(expr)>;
    using MExpr = typename std::decay_t<decltype(m_expr_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::mixed_multiplication_filler<NRows, NCols, Expr, MExpr>{ std::move(expr), std::move(m_expr_p)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t NCols, class... Exprs, class T, class Enabler = std::enable_if_t<std::is_arithmetic<T>::value> >
constexpr auto operator*( expr::matrix_expr<NRows, NCols, Exprs ...> && m_expr_p,
                          T const& t_p)
{
    // puts(__PRETTY_FUNCTION__);
    auto&& expr = expr::terminal_expr<T>( t_p );
    using Expr = typename std::decay_t<decltype(expr)>;
    using MExpr = typename std::decay_t<decltype(m_expr_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::mixed_multiplication_filler<NRows, NCols, Expr, MExpr>{ std::move(expr), std::move(m_expr_p)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t N, std::size_t NCols >
constexpr auto operator*( matrix<NRows, N> const& m1_p,
                          matrix<N, NCols> const& m2_p )
{
    // puts(__PRETTY_FUNCTION__);
    auto&& m_expr1 = expr::make_matrix_expr_impl<NRows, N>( expr::terminal_filler<NRows, N>(m1_p), std::make_index_sequence<NRows * N>() );
    auto&& m_expr2 = expr::make_matrix_expr_impl<N, NCols>( expr::terminal_filler<N, NCols>(m2_p), std::make_index_sequence<N * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr1)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr2)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::matrix_multiplication_filler<NRows, N, NCols, MExpr1, MExpr2>{ std::move(m_expr1), std::move(m_expr2)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t N, std::size_t NCols, class... Exprs >
constexpr auto operator*( expr::matrix_expr<NRows, N, Exprs...> && m_expr_p,
                          matrix<N, NCols> const& m_p )
{
    auto&& m_expr = expr::make_matrix_expr_impl<N, NCols>( expr::terminal_filler<N, NCols>(m_p), std::make_index_sequence<N * NCols>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr_p)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::matrix_multiplication_filler<NRows, N, NCols, MExpr1, MExpr2>{ std::move(m_expr_p), std::move(m_expr)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t N, std::size_t NCols, class... Exprs>
constexpr auto operator*( matrix<NRows, N> const& m_p,
                          expr::matrix_expr<N, NCols, Exprs...> && m_expr_p )
{
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, N>( expr::terminal_filler<NRows, N>(m_p), std::make_index_sequence<NRows * N>() );
    using MExpr1 = typename std::decay_t<decltype(m_expr)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                                expr::matrix_multiplication_filler<NRows, N, NCols, MExpr1, MExpr2>{ std::move(m_expr), std::move(m_expr_p)} ,
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


template< std::size_t NRows, std::size_t N, std::size_t NCols, class ... Exprs1, class ... Exprs2 >
constexpr auto operator*( expr::matrix_expr<NRows, N, Exprs1...> && m_expr1_p,
                          expr::matrix_expr<N, NCols, Exprs2...> && m_expr2_p )
{
    using MExpr1 = typename std::decay_t<decltype(m_expr1_p)>;
    using MExpr2 = typename std::decay_t<decltype(m_expr2_p)>;
    return expr::make_matrix_expr_impl<NRows, NCols>(
                            expr::matrix_multiplication_filler<NRows, N, NCols, MExpr1, MExpr2>{ std::move(m_expr1_p), std::move(m_expr2_p)} ,
                            std::make_index_sequence<NRows * NCols>()
                                                     );
}




//--------------------------TRANSPOSE---------------------------

template< std::size_t NRows, std::size_t NCols>
constexpr auto transpose(matrix<NRows, NCols> const& m_p)
{
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    return expr::make_matrix_expr_impl<NCols, NRows>(
                            expr::transpose_filler<NRows, NCols, std::decay_t<decltype(m_expr)>>{ std::move(m_expr) },
                            std::make_index_sequence<NRows * NCols>()
                                                     );
}

template< std::size_t NRows, std::size_t NCols, class ... Exprs>
constexpr auto transpose( expr::matrix_expr<NRows, NCols, Exprs...>&& m_expr_p)
{
    return expr::make_matrix_expr_impl<NCols, NRows>(
                                expr::transpose_filler<NRows, NCols, std::decay_t<decltype(m_expr_p)>>{ std::move(m_expr_p) },
                                std::make_index_sequence<NRows * NCols>()
                                                     );
}


//--------------------------ROW & COLUMN---------------------------

template< std::size_t IRow, std::size_t NRows, std::size_t NCols>
constexpr auto row(matrix<NRows, NCols> const& m_p)
{
    //might need row_terminal_filler -> to skip unnecessary work ?
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    return expr::make_matrix_expr_impl<1, NCols>(
                                expr::row_filler<IRow, std::decay_t<decltype(m_expr)>>{ std::move(m_expr) },
                                std::make_index_sequence< NCols>()
                                                 );
}

template< std::size_t IRow, std::size_t NRows, std::size_t NCols, class ... Exprs>
constexpr auto row( expr::matrix_expr<NRows, NCols, Exprs...>&& m_expr_p)
{
    return expr::make_matrix_expr_impl<1, NCols>(
                                expr::row_filler<IRow, std::decay_t<decltype(m_expr_p)>>{ std::move(m_expr_p) },
                                std::make_index_sequence< NCols >()
                                                 );
}

template< std::size_t ICol, std::size_t NRows, std::size_t NCols>
constexpr auto column(matrix<NRows, NCols> const& m_p)
{
    //might need row_terminal_filler -> to skip unnecessary work ?
    auto&& m_expr = expr::make_matrix_expr_impl<NRows, NCols>( expr::terminal_filler<NRows, NCols>(m_p), std::make_index_sequence<NRows * NCols>() );
    return expr::make_matrix_expr_impl<NRows, 1>(
                                expr::column_filler<ICol, std::decay_t<decltype(m_expr)>>{ std::move(m_expr) },
                                std::make_index_sequence< NRows>()
                                                 );
}

template< std::size_t ICol, std::size_t NRows, std::size_t NCols, class ... Exprs>
constexpr auto column(expr::matrix_expr<NRows, NCols, Exprs...>&& m_expr_p)
{
    return expr::make_matrix_expr_impl<NRows, 1>(
                                expr::column_filler<ICol, std::decay_t<decltype(m_expr_p)>>{ std::move(m_expr_p) },
                                std::make_index_sequence< NRows >()
                                                 );
}

#endif /* expr_hpp */
