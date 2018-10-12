%%% DisplayHeatmap %%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file takes the arrays created in the file SeparateSignals and plots
% them in an overlay atop the blueprints of the building in the area in
% which we gathered data. The heatmap then accurately represents the number
% of signal carriers that are a specified number of dB above the noise
% floor. Both transmitter locations are shown.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Change these limits to set the thresholds for the heatmap
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tx1_dblimit = 12;
tx2_dblimit = 25;

save('tx1_dblimit.mat','tx1_dblimit');
save('tx2_dblimit.mat','tx2_dblimit');

runsignals = false;
runheatmap = true;
runcapacity = true;

if runsignals == true
    SeparateSignals;
end

if (runheatmap)
    plotHeatmap();
end

if (runcapacity)
    plotCapacity();
end

function plotHeatmap()
% Check for the variables that are needed in the workspace
if exist('complete_carriers_above','var') == 0
    load('complete_carriers_above');
end
if exist('tx2_carriers_above','var') == 0
    load('tx2_carriers_above');
end
if exist('tx1_dblimit','var') == 0
    load('tx1_dblimit');
end
if exist('tx2_dblimit','var') == 0
    load('tx2_dblimit');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% First Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

close all;
I = imread('Clyde4thFloor.png');
% subplot(2,1,1);
figure();
imshow(I);
hold on
text(99, 323, '*', 'Color', 'red', 'FontSize', 16);
text(104, 304, 'tx', 'Color', 'red', 'FontSize', 7);
hm = imagesc(complete_carriers_above);
title({'\fontsize{12}First Transmitter Location'; ...
    '\fontsize{7}# of carriers ' + string(tx1_dblimit) + ...
    ' dB above noise floor'});
set(hm,'AlphaData',~isnan(complete_carriers_above));
q = colorbar;
colormap(jet);
ylabel(q, '# of carriers above noise floor');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Second Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% subplot(2,1,2);
figure();
imshow(I);
hold on
text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
text(110, 178, 'tx', 'Color', 'red', 'FontSize', 7);
hm = imagesc(tx2_carriers_above);
title({'\fontsize{12}Second Transmitter Location'; ...
    '\fontsize{7}# of carriers ' + string(tx2_dblimit) + ...
    ' dB above noise floor'});
set(hm,'AlphaData',~isnan(tx2_carriers_above));
q = colorbar;
colormap(jet);
ylabel(q, '# of carriers above noise floor');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off
end

function plotCapacity()
load('tx1_linear_signal.mat');
load('tx1_linear_noisefloor.mat');
load('tx2_linear_signal.mat');
load('tx2_linear_noisefloor.mat');

for x = 1:64
    for y = 1:90
        for z = 1:345
            tx1_noise(x,y,z) = sum(tx1_linear_noisefloor(:,y,z));
            tx2_noise(x,y,z) = sum(tx2_linear_noisefloor(:,y,z));
        end
    end
end
tx1_noise = tx1_noise / 64;
tx2_noise = tx2_noise / 64;
% save('tx2_noise.mat','tx2_noise');
tx1_cpcty = (1/2) * log2(1 + (tx1_linear_signal ./ tx1_noise));
tx2_cpcty = (1/2) * log2(1 + (tx2_linear_signal ./ tx2_noise));
for y = 1:90
    for z = 1:345
        tx1_capacity(y,z) = sum(tx1_cpcty(:,y,z));
        tx2_capacity(y,z) = sum(tx2_cpcty(:,y,z));
    end
end
save('tx1_capacity.mat','tx1_capacity');
save('tx2_capacity.mat','tx2_capacity');

I = imread('Clyde4thFloor.png');

figure();
imshow(I);
hold on
text(99, 323, '*', 'Color', 'red', 'FontSize', 16);
text(104, 304, 'tx', 'Color', 'red', 'FontSize', 7);
hm = imagesc(tx1_capacity);
title({'First Transmitter Location Capacity'});
set(hm,'AlphaData',~isnan(tx1_capacity));
q = colorbar;
colormap(jet);
ylabel(q, 'Capacity');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off

figure();
imshow(I);
hold on
text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
text(110, 178, 'tx', 'Color', 'red', 'FontSize', 7);
hm = imagesc(tx2_capacity);
title({'Second Transmitter Location Capacity'});
set(hm,'AlphaData',~isnan(tx2_capacity));
q = colorbar;
colormap(jet);
ylabel(q, 'Capacity');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off
end

