function x = pwm(dat, delta)
x = zeros(1, length(dat));
sum = 0;
for i = 1:length(dat)
    d = dat(i);
    out = delta * sign(d-sum);
    sum += out;
    x(i) = sign(d-sum) / 2 + 0.5;
endfor
endfunction
