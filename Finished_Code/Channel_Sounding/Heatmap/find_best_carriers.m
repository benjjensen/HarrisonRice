% Created by Dakota Flanary
function [best_carriers] = find_best_carriers(carrier_map,data_map,magic_number,threshold)
%find_best_carriers Finds the specific carriers that are above the
%threshold
%   Finds the specific carriers where the data on the carrier is more than
%   the threshold above the noise on that carrier
%   Returns an array where the non-zero indices represent the good carriers
carriers = zeros(1,64);
[~,index] = max(carrier_map);
[~,num_runs,~] = size(data_map);
row = floor((index-1)/num_runs) + 1;
run = mod((index-1),num_runs) + 1;
for loc = 1:2:127
    if data_map(row,run,loc) - data_map(row,run,loc+1) > threshold
        carriers(1,(loc+1)/2) = magic_number;
    end
end
best_carriers = carriers;
end

