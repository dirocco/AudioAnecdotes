function y = clip(x, lo, hi)
if (x > hi)
  y = hi;
elseif (x < lo)
  y = lo;
else
  y = x;
end
end
