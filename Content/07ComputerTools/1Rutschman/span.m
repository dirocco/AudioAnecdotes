function x = span(dat, width)
x = zeros(1, columns(dat) * width);
for i = 1:length(dat)
  for j = 1:width
    x((i-1)*width + j) = dat(i);
  endfor
endfor
endfunction
