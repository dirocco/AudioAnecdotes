function x = pwm2(dat, width)
x = zeros(1, length(dat) * width);
for i = 1:length(dat)
  count = floor (0.5+width * (clip(dat(i),-1,1) / 2 + 0.5));
  for j = 1:width
    out = (j <= count);
    x((i-1)*width + j) = count;
  endfor
endfor
endfunction
