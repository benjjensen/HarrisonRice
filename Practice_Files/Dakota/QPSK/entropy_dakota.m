function [h] = entropy_dakota(p)
%entropy_dakota calculates the entropy
%   if p = 0 then the entropy is 0 else it is -p * log2(p)
if p == 0
    h = 0;
else
    h = -p .* log2(p);
end

