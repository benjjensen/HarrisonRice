function [total_capapcity,carriers_capacity] = gaussian_capacity(data,snr)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
carriers_capacity = (1/2) .* log2(1 + (data .* snr));
[num_columns,~] = size(data);
total_capapcity = zeros(1,num_columns);
    for col = 1:num_columns
        if ~isnan(carriers_capacity(col))
            total_capapcity(col) = sum(carriers_capacity(col,:));
        end
    end
end