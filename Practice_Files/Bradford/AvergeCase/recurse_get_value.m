function [pr] = recurse_get_value(pr_mat,n)
%recurse_get_value Recursively breaks the probability array into smaller
%groups and then combines those groups to find the probability distribution
%   Takes the current group and splits it into two groups of equal length,
%   if the current group is already size 1 then it returns in the first
%   index 1 - the probability and the second index is the original
%   probability.  After getting the two groups it cycles through the groups
%   to get the distribution on the groups.
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

