/* Meta-programming utility.
   Copyright (C) 2013 Jarryd Beck

This file is part of Juice.

Distributed under the Boost Software License, Version 1.0

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

  The copyright notices in the Software and this entire statement, including
  the above license grant, this restriction and the following disclaimer,
  must be included in all copies of the Software, in whole or in part, and
  all derivative works of the Software, unless such copies or derivative
  works are solely in the form of machine-executable object code generated by
  a source language processor.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
  SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
  FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

*/

#ifndef JUICE_MPL_HPP_INCLUDED
#define JUICE_MPL_HPP_INCLUDED

#include <type_traits>
#include <cstdlib>

namespace Juice
{

    namespace detail
    {
        template
        <
        template <typename> class Size,
                 typename SoFar,
                 typename... Args
                 >
        struct max_helper;

        template
        <
        template <typename> class Size,
                 typename SoFar
                 >
        struct max_helper<Size, SoFar>
        {
            static constexpr decltype(Size<SoFar>::value) value =
                Size<SoFar>::value;
            typedef SoFar type;
        };

        template
        <
        template <typename> class Size,
                 typename SoFar,
                 typename Next,
                 typename... Args
                 >
        struct max_helper<Size, SoFar, Next, Args...>
        {
private:
            typedef typename std::conditional
            <
            (Size<Next>::value > Size<SoFar>::value),
            max_helper<Size, Next, Args...>,
            max_helper<Size, SoFar, Args...>
            >::type m_next;

public:
            static constexpr decltype(Size<SoFar>::value) value =
                m_next::value;

            typedef typename m_next::type type;
        };
    }

    template <template <typename> class Size, typename... Args>
    struct max;

    template
    <
    template <typename> class Size,
             typename First,
             typename... Args
             >
    struct max<Size, First, Args...>
    {
private:
        typedef decltype(Size<First>::value) m_size_type;
        typedef detail::max_helper
        <
        Size,
        First,
        Args...
        > m_helper;

public:
        static constexpr m_size_type value = m_helper::value;
        typedef typename m_helper::type type;
    };

}

#endif