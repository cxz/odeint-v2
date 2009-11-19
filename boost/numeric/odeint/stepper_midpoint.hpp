/* Boost odeint/stepper_midpoint.hpp header file
 
 Copyright 2009 Karsten Ahnert
 Copyright 2009 Mario Mulansky
 
 This file includes the explicit midpoint solver for
 ordinary differential equations.

 It solves any ODE dx/dt = f(x,t) via
 x(t+dt) = x(t) + dt*f(x,t)

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_NUMERIC_ODEINT_STEPPER_MIDPOINT_HPP
#define BOOST_NUMERIC_ODEINT_STEPPER_MIDPOINT_HPP

#include <boost/concept_check.hpp>

#include <boost/numeric/odeint/detail/iterator_algebra.hpp>
#include <boost/numeric/odeint/concepts/state_concept.hpp>
#include <boost/numeric/odeint/resizer.hpp>



namespace boost {
namespace numeric {
namespace odeint {

    template<
        class Container ,
        class Time = double ,
        class Resizer = resizer< Container >
        >
    class stepper_midpoint
    {
        // provide basic typedefs
    public:

        typedef Container container_type;
        typedef Resizer resizer_type;
        typedef Time time_type;
        typedef const unsigned short order_type;
        typedef typename container_type::value_type value_type;
        typedef typename container_type::iterator iterator;




        // check the concept of the ContainerType
    private:

        BOOST_CLASS_REQUIRE( container_type ,
			     boost::numeric::odeint, Container );

        
        // private memebers
    private:
        resizer_type m_resizer;

        container_type m_x0;
        container_type m_x1;
        container_type m_dxdt;

    public:
        
        order_type order() const { return 2; }

        template< class DynamicalSystem >
        void next_step( 
                DynamicalSystem &system ,
                container_type &x ,
                container_type &dxdt ,
                time_type t ,
                time_type dt ,
                unsigned short n = 2 )
        {
            if( n < 2 ) return;

            const time_type h = dt/static_cast<time_type>( n );
            const time_type h2 = static_cast<time_type>( 2.0 )*h;
            const time_type t_1 = static_cast<time_type>( 1.0 );
            const time_type t_05 = static_cast<time_type>( 0.5 );
            time_type th = t + h;

            m_resizer.adjust_size(x, m_x0);
            m_resizer.adjust_size(x, m_x1);
            m_resizer.adjust_size(x, m_dxdt);

            using namespace detail::it_algebra;

            // m_x0 = x + h*dxdt
            scale_sum( m_x0.begin(), m_x0.end(),
                       t_1, x.begin(),
                       h, dxdt.begin() );
            system( m_x0, m_dxdt, th );

            m_x1 = x;

            unsigned short i = 1;
            while( i != n )
            {   // general step
                //tmp = m_x1; m_x1 = m_x0 + h2*m_dxdt; m_x0 = tmp
                scale_sum_swap( m_x1.begin(), m_x1.end(), 
                                m_x0.begin(),
                                h2, m_dxdt.begin() );
                th += h;
                system( m_x1, m_dxdt, th);
                i++;
            }
            
            // last step
            // x = 0.5*( m_x0 + m_x1 + h*m_dxdt
            scale_sum( x.begin(), x.end(),
                       t_05, m_x0.begin(),
                       t_05, m_x1.begin(),
                       t_05*h, m_dxdt.begin() );
        }


        template< class DynamicalSystem >
        void next_step( 
                DynamicalSystem &system ,
                container_type &x ,
                time_type t ,
                time_type dt ,
                unsigned short n = 2 )
        {
            m_resizer.adjust_size(x, m_dxdt);
            system( x, m_dxdt, t );
            next_step( system , x, m_dxdt, t, dt, n );
        }
            

    };

}
}
}

#endif
