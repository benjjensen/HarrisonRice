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

runsignals = true;

if runsignals == true
    SeparateSignals;
end

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
% title({'\fontsize{12}Second Transmitter Location'; ...
%     '\fontsize{7}# of carriers ' + string(tx2_dblimit) + ...
%     ' dB above noise floor'});
set(hm,'AlphaData',~isnan(tx2_carriers_above));
q = colorbar;
colormap(jet);
ylabel(q, '# of carriers above noise floor');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off


