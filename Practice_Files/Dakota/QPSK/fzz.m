function [y] = fzz(eps,x,G)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
l = length(x);
for i = 1:l
    if x(i) ==  0
        y(i) = inf;
    else
        for j = 0:G/100:10*G
            if eps(x(i),j) == 0
                y(i) = j;
                break
            else
                y(i) = inf;
            end
        end
    end
end