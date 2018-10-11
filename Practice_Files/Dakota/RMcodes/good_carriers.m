function [good_ones] = good_carriers(pr_mat)
%UNTITLED3 Summary of this function goes here
% % Finds the indices where the probability is greater than 95%
[num_rows,num_cols] = size(pr_mat);
indices = [];
for row = 1:num_rows
    index = 0;
    for col = 1:num_cols
        if pr_mat(row,col) >= .95
            index = index + 1;
            indices(row,index) = col;
        end
    end
end
good_ones = indices;
end

