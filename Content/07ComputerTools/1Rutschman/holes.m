function x = holes(dat, width)
x = zeros(1, length(dat) * width);
for i = 1:length(dat)
    x((i-1)*width + 1) = dat(i);
endfor
endfunction
