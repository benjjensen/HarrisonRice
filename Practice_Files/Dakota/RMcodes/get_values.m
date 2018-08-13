function [v] = get_values(pr_mat,n,k)
%get_values Finds the probability of k carriers being good 
%   Goes through all of the n choose k combinations and sums up the
%   probability of it occuring
if(k == 0)
    v = prod(1 - pr_mat);
elseif(k == n)
    v = prod(pr_mat);
else
    total = 0;
    combos = nchoosek(pr_mat,k);
    for i = 1:nchoosek(n,k)
        temp = union(combos(i,:),pr_mat,'stable');
        [~,s] = size(temp);
        if s ~= k
            
        end
        temp = 1-temp;
        for j = 1:k
            temp(j) = 1-temp(j);
        end
        total = total + prod(temp);
    end
    v = total;
end
end

