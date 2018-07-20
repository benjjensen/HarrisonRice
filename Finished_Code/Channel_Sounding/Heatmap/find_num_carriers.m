function [carrier_list] = find_num_carriers(data,threshold)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[num_rows,num_runs,~] = size(data);
num_carrier_list = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
% %         If you want to find it based on the average noise floor
% %         uncomment this code.
%         noise_floor = 0;
%         count = 0;
%         for loc = 19:2:110
% 
%             noise_floor = noise_floor + data(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = data(row,run,loc) - noise_floor;
%                 if(dif > threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        if data(row,run,1) == 0
            num_carriers = nan;
        else
            for loc = 1:2:127
                if data(row,run,loc) - data(row,run,loc+1) > threshold
                    num_carriers = num_carriers + 1;
                end
            end
            spot = spot + 1;
            num_carrier_list(spot,1) = num_carriers;
        end
    end
end
carrier_list = num_carrier_list;
end

