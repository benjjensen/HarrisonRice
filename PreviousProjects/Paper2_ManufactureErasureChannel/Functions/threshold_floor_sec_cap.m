% Created by Dakota Flanary
function [carrier_list,room] = threshold_floor_sec_cap(bob,data,snr,threshold)
%threshold_find_num_carriers Finds the number of carriers above the specified
%                   threshold
%   Compares each carrier with the noise on the carrier and if the
%   difference is greater than the threshold it is counted as a good
%   carrier. If there is no data a nan is inputed for the number of
%   carriers. Stores the number of carriers in a single column to make
%   finding the max easier, also stores it in the grid form that represents
%   the room
[~,num_rows,num_runs] = size(data);
carrier_list = zeros(num_rows*num_runs,1); % initializes the column vector to 0s
index = 0; % initializes the index counter
row_carriers = zeros(num_runs,1);
room = nan(num_rows,num_runs); % room representation of the carriers 
for row = 1:num_rows % goes through eah row
    for run = 1:num_runs % and each column in the room
        num_carriers = 0; % count of the number of carriers in that specific row,column square
        index = index + 1; % increments the index
        if isnan(data(1,row,run)) || data(1,row,run) == 0
            num_carriers = nan; % if the value is nan then it stays nan
        else
            for carrier = 1:45 % goes throught the 45 good carriers
                if bob(carrier) ~= 0
                    if data(carrier,row,run) * snr >= threshold % if the g^2 * snr is >= our threshold
                        num_carriers = num_carriers + 1; % the carrier is considered to be good
                    end
                end
            end
            carrier_list(index,1) = num_carriers; % the current index of the list of num_carriers is = num_carriers in the spot
        end
        row_carriers(run) = num_carriers; % the current run in the row is = num_carriers in the spot
    end
    room(row,1:num_runs) = row_carriers; % the current row of carriers is = to the row_carriers variable
end
end

