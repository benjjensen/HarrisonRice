% Created by Dakota Flanary
function [best_carriers] = find_best_carriers(data_map,threshold)
%find_best_carriers Finds the specific carriers that are above the
%threshold
%   Finds the specific carriers where the data on the carrier is more than
%   the threshold above the noise on that carrier
%   Returns an array where the non-zero indices represent the good carriers
[~,num_runs] = size(data_map);
carriers = zeros(64,num_runs);
for run = 1:num_runs 
    for loc = 1:2:127
        if data_map(loc,run) - data_map(loc+1,run) > threshold
            carriers((loc+1)/2,run) = true;%(loc+1)/2;
        end
    end
end

best_carriers = carriers;

end
