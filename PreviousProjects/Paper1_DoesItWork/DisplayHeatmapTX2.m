%% DisplayHeatmapTX2 
clear; close all;

%% Separate Signals

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This section is used to separate the pwelch data into the noise floor and
% signal carriers, after which the difference (in dB) between the two is
% found and a new array is created that represents the number of carriers
% at each given location that are above a specified number of dB from the
% noise floor.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

load Data/tx2dB.mat; % Run CombineAndProcessArraysTX2.m to create this variable

dblimit = 27; % Change this to set the threshold for the heatmap
startCarrier = 10;
endCarrier = 54;

tx2_signal = nan(64,91,345);
tx2_noisefloor = nan(64,91,345);
difference = nan(64,91,345);
tx2_snr_db = nan(64,91,345);
tx2_carriers_above = nan(91,345);
for temp = 1:91
    for loops = 1:345
        tx2_signal(:,temp,loops) = tx2dB(1:2:end,temp,loops);
        tx2_noisefloor(:,temp,loops) = tx2dB(2:2:end,temp,loops);
        if isnan(tx2_signal(:,temp,loops))
            difference(:,temp,loops) = nan;
        else
            difference(:,temp,loops) = abs(tx2_signal(:,temp,loops) - tx2_noisefloor(:,temp,loops));
        end
        count = 0;
        for carriers = 10:54
            tx2_noise_avg(carriers,temp,loops) = sum(tx2_noisefloor(:,temp,loops));
            tx2_noise_avg(carriers,temp,loops) = (tx2_noise_avg(carriers,temp,loops) ...
                                                / (endCarrier - startCarrier) + dblimit);
            if isnan(tx2_signal(carriers,temp,loops))
            else
                tx2_snr_db(carriers,temp,loops) = tx2_signal(carriers,temp,loops) - tx2_noise_avg(carriers,temp,loops);
            end
            if difference(carriers,temp,loops) >= dblimit
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

%% Display Heatmap

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This section takes the arrays created above and plots them in an overlay
% atop the blueprints of the building in the area in which we gathered
% data. The heatmap then accurately represents the number of signal 
% carriers that are a specified number of dB above the noise floor.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

I = imread('Figures/Clyde4thFloor.png');
figure();
imshow(I);
hold on
text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
text(110, 178, 'tx', 'Color', 'red', 'FontSize', 7);
hm = imagesc(tx2_carriers_above);
title({'\fontsize{12}Second Transmitter Location'; ...
    '\fontsize{7}# of carriers ' + string(dblimit) + ...
    ' dB above noise floor'});
set(hm,'AlphaData',~isnan(tx2_carriers_above));
q = colorbar;
colormap(jet);
ylabel(q, '# of carriers above noise floor');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off