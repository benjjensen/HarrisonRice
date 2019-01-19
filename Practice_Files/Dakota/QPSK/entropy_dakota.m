function [h] = entropy_dakota(p)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
if p == 0
    h = 0;
else
    h = -p .* log2(p);
end

