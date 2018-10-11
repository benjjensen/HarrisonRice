%%% SeparateSignals %%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file is used to separate the pwelch data into the noise floor and
% signal carriers, after which the difference (in dB) between the two is
% found and a new array is created that represents the number of carriers
% at each given location that are above a specified number of dB from the
% noise floor.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if exist('tx1_dblimit','var') == 0
    load('tx1_dblimit');
end
if exist('tx2_dblimit','var') == 0
    load('tx2_dblimit');
end
if exist('complete_pwelch','var') == 0
    load('complete_pwelch');
end
if exist('tx2_pwelch','var') == 0
    load('tx2_pwelch');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% First Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
complete_signal = nan(64,90,345);
complete_noisefloor = nan(64,90,345);
difference = nan(64,90,345);
complete_carriers_above = nan(90,345);

for temp = 1:90
    for loops = 1:345
        complete_signal(:,temp,loops) = complete_pwelch(1:2:end,temp,loops);
        complete_noisefloor(:,temp,loops) = complete_pwelch(2:2:end,temp,loops);
        if isnan(complete_signal(:,temp,loops))
            difference(:,temp,loops) = nan;
        else
            difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
        end
        count = 0;
        for carriers = 1:64
            if difference(carriers,temp,loops) >= tx1_dblimit
                count = count + 1;
            end
        end
        if isnan(difference(:,temp,loops))
            complete_carriers_above(temp,loops) = nan;
        else
            complete_carriers_above(temp,loops) = count;
        end
    end
end

save('complete_carriers_above.mat','complete_carriers_above');
save('complete_signal.mat','complete_signal');
save('complete_noisefloor.mat','complete_noisefloor');

clear difference;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Second Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tx2_signal = nan(64,90,345);
tx2_noisefloor = nan(64,90,345);
difference = nan(64,90,345);
tx2_carriers_above = nan(90,345);

for temp = 1:90
    for loops = 1:345
        tx2_signal(:,temp,loops) = tx2_pwelch(1:2:end,temp,loops);
        tx2_noisefloor(:,temp,loops) = tx2_pwelch(2:2:end,temp,loops);
        if isnan(tx2_signal(:,temp,loops))
            difference(:,temp,loops) = nan;
        else
            difference(:,temp,loops) = abs(tx2_signal(:,temp,loops) - tx2_noisefloor(:,temp,loops));
        end
        count = 0;
        for carriers = 1:64
            if difference(carriers,temp,loops) >= tx2_dblimit
                count = count + 1;
            end
        end
        if isnan(difference(:,temp,loops))
            tx2_carriers_above(temp,loops) = nan;
        else
            tx2_carriers_above(temp,loops) = count;
        end
    end
end

save('tx2_carriers_above.mat','tx2_carriers_above');
save('tx2_signal.mat','tx2_signal');
save('tx2_noisefloor.mat','tx2_noisefloor');



