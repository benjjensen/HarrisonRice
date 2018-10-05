function [v] = get_values(pr_mat,n)
%get_values Finds the probability of k carriers being good 
%   Receives an array of probabilities and parameters n and k, n being the 
%   block length and k being the number of revealed bits.  It then goes 
%   through all of the n choose k combinations and calculates the
%   probability of that n and k occuring
pr = zeros(1,n+1);
bin_arr = false(1,n);
iter = 0;

while ~isequal(bin_arr(1:n),true(1,n))
%     tic;
    iter = iter + 1;
    if iter == 1000000
        disp(bin_arr);
        iter = 0;
    end
    temp = pr_mat(bin_arr);
    temp2 = 1 - pr_mat(~bin_arr);
    if isempty(temp)
        temp = temp2;
    elseif ~isempty(temp2)
        temp = cat(2,temp,temp2);
    end
    k = sum(bin_arr);
    pr(k+1) = pr(k+1) + prod(temp);
    plus1 = true;
    i = 0;
    while plus1 == true
        index = n-i;
        if bin_arr(index) == true
            bin_arr(index) = false;
%         bin_arr(index) = bin_arr(index) + 1;
%         if bin_arr(index) == 2
%             bin_arr(index) = 0;
        else
            bin_arr(index) = true;
            plus1 = false;
        end
        i = i + 1;
    end
%     toc;
end
pr(n+1) = prod(pr_mat);
v = pr;
% do one more calculation when all are good
% 
% 
% 
% if(k == 0)
%     v = prod(1 - pr_mat);
% elseif(k == n)
%     v = prod(pr_mat);
% else
%     total = 0;
%     [~,num_cols] = size(pr_mat);
%     ss = 1:num_cols;
%     combos = nchoosek(ss,k);
%     parfor i = 1:nchoosek(n,k)
%         temp = zeros(1,n);
%         tt = ss;
% %         [~,num_cols] = size(combos);
%         for col = 1:k
%             tt(tt == combos(i,col)) = [];
%         end
%         dd = cat(2,combos(i,:),tt);
% %         dd = setdiff(ss, combos(i,:));    
%         for j = 1:k
%             temp(1,j) = pr_mat(dd(j)); 
%         end
%         for j = k+1:n
%             temp(1,j) = 1 - pr_mat(dd(j));
%         end
% %         temp = 1-temp;
% %         for j = 1:k
% %             temp(j) = 1-temp(j);
% %         end
%         total = total + prod(temp);
%     end
%     v = total;
% end
end

