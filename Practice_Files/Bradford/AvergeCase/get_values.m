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
    [~,num_cols] = size(pr_mat);
    ss = 1:num_cols;
    combos = nchoosek(ss,k);
    for i = 1:nchoosek(n,k)
        temp = zeros(1,n);
        dd = setdiff(ss, combos(i));    
        for j = 1:k
            temp(1,j) = pr_mat(ss(j)); 
        end
        for j = 1:n-k
            temp(1,j+k) = pr_mat(dd(j));
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

