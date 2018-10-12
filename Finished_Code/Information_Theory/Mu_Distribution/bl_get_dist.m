function [pr_dist] = bl_get_dist(pr_mat)
%bl_get_dist gets the distribution of all of the combos and averages the
%probabilities to get the average distribution
%   input is a matrix of the possible combinations with each row being a
%   different combination of carriers, each combination is passed into the get_values
%   function where the probability for each number of revealed bits is
%   calculated, if there is more than one combination the probabilities are
%   averaged which becomes the output.
[num_rows,n] = size(pr_mat);
probabilities = zeros(num_rows,(n+1));
for row = 1:num_rows
    
%     for k = 0:n
        tic;
%         disp(k);
%         probabilities(row,:) = get_values(pr_mat(row,:),n);
    probabilities(row,:) = recurse_get_value(pr_mat(row,:),n);
        toc;
%     end
end
if num_rows ~= 1
    pr_dist = sum(probabilities)/num_rows;
else
    pr_dist = probabilities;
end
end
