function y = quantize(x,bits)
y = (floor(0.5+2**bits*((x+1.0)/2))/2**bits)*2-1;
end
