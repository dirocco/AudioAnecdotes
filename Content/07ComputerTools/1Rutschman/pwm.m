function x = pwm(dat, width)
x = zeros(1, length(dat) * width);
for i = 1:length(dat)
  count = round (width * (clip(dat(i),-1,1) / 2 + 0.5));
  for j = 1:width
    out = (j <= count);
    x((i-1)*width + j) = out*2-1;
  endfor
endfor
endfunction
