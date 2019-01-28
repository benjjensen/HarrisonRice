function [carrier_list] = find_num_carriers(data,threshold)
    %find_num_carriers Finds the number of carriers above the specified
    %                   threshold
    %   Compares each carrier with the noise on the carrier and if the
    %   difference is greater than the threshold it is counted as a good
    %   carrier.  If there is no data a nan is inputed for the number of
    %   carriers.  Alternate method of finding the average noise floor is done
    %   by uncommenting the commented code and commenting out the current for
    %   loop where the variable is loc.
    [~,num_runs] = size(data);
    num_carrier_list = zeros(num_runs,1);
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

    %%% If you want it based on the individual noise floor use this 
        if data(1,run) == 0
            num_carriers = nan;
        else
            for loc = 1:2:127
                if data(loc,run) - data(loc+1,run) > threshold
                    num_carriers = num_carriers + 1;
                end
            end
            num_carrier_list(run,1) = num_carriers;
        end
    end
    
    carrier_list = num_carrier_list;
end

