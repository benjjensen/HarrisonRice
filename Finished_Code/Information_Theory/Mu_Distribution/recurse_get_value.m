function [pr] = recurse_get_value(pr_mat,n)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
pr = zeros(1,n+1);
bl = n/2;
if n == 1
    pr(1) = 1-pr_mat(1);
    pr(2) = pr_mat(1);
else
    pr_a = recurse_get_value(pr_mat(1,1:bl),bl);
    pr_b = recurse_get_value(pr_mat(1,1 + bl:end),bl);
    for i = 0:bl
        for j = 0:bl
            k = i + j;
            pr(k+1) = pr(k+1) + pr_a(i+1)*pr_b(j+1);
        end
    end
end
end

