function x = dsm(dat, width)
x = zeros(1, length(dat) * width);
sum = 0;
for i = 1:length(dat)
  for j = 1:width
    d = dat(i);
    delta = d - sum;
    out = sign(delta);
    sum += out/width;
    x((i-1)*width + j) = out;
  endfor
endfor
endfunction
