## Copyright (C) 2000 Paul Kienzle
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301, USA.

## -*- texinfo -*-
## @deftypefn {Function File} {} factorial (@var{n})
## Return the factorial of @var{n}. If @var{n} is scalar, this is
## equivalent to @code{prod (1:@var{n})}.  If @var{n} is an array,
## the factorial of the elements of the array are returned.
## @end deftypefn

function x = factorial (n)
  if (nargin != 1)
    print_usage ();
  elseif (any (n(:) < 0 | n(:) != round (n(:))))
    error ("factorial: n must all be nonnegative integers");
  endif
  x = round (gamma (n+1));
endfunction

%!assert (factorial(5), prod(1:5))
%!assert (factorial([1,2;3,4]), [1,2;6,24])
%!assert (factorial(70), exp(sum(log(1:70))), -10*eps)
%!fail ('factorial(5.5)', "must all be nonnegative integers")
%!fail ('factorial(-3)', "must all be nonnegative integers")
