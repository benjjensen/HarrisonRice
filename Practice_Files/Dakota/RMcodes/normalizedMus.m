function [norm_mu] = normalizedMus(mu_pmf)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[num_rows,num_cols] = size(mu_pmf);
% sum = 0;
% num_pass = 1;
% % norm_mu = zeros(1,num_cols);
% for row = 1:num_rows
%     for col = 1:num_cols
%         sum = sum + mu_pmf(row,col);
%         if((col-1)/(num_cols-1) >= num_pass*.01)
% %             norm_mu(col) = (col-1)/(num_cols-1);
%             norm_mu(num_pass) = num_pass*.01;
%             mu_two(row,num_pass) = sum;
%             sum = 0;
%             num_pass = num_pass + 1;
%         end
%     end
%     num_pass = 1;
% end
% end
for col = 1:num_cols
    norm_mu(col) = (col-1)/(num_cols-1);
end
end
