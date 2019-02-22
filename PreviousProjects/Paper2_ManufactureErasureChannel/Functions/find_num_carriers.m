% Created by Dakota Flanary
function [carrier_list,floor] = find_num_carriers(data,snr,threshold)
%find_num_carriers Finds the number of carriers above the specified
%                   threshold
%   Compares each carrier with the noise on the carrier and if the
%   difference is greater than the threshold it is counted as a good
%   carrier.  If there is no data a nan is inputed for the number of
%   carriers.  Alternate method of finding the average noise floor is done
%   by uncommenting the commented code and commenting out the current for
%   loop where the variable is loc.
[~,num_rows,num_runs] = size(data);
num_carrier_list = zeros(num_rows*num_runs,1);
spot = 0;
row_carriers = zeros(num_runs,1);
room = nan(num_rows,num_runs);
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        spot = spot + 1;
        if isnan(data(1,row,run))
            num_carriers = nan;
        else
            for loc = 1:45
                if data(loc,row,run) * snr >= threshold
                    num_carriers = num_carriers + 1;
                end
            end
            num_carrier_list(spot,1) = num_carriers;
        end
        row_carriers(run) = num_carriers;
    end
    room(row,1:num_runs) = row_carriers;
end
carrier_list = num_carrier_list;
floor = room;
end

