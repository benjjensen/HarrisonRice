clear;
close all;
% 
% load linear_signal.mat;
load linear_noisefloor.mat;
% signal = linear_signal;
% noise = linear_noisefloor;

load linear_signal.mat;
for y = 1:90
    for z = 1:345
        linear_signal(:,y,z) = fftshift(linear_signal(:,y,z));
    end
end

gc_signal = linear_signal(11:55,:,:);
g = sqrt(gc_signal);
bob = g(:,36:65,65:98);
g_norm = g ./ max(max(max(bob)));
g_squared = g_norm .^ 2;
signal = g_squared;
noise = linear_noisefloor(10:54,:,:);
for y = 1:90
    for z = 1:345
        noise(:,y,z) = sum(noise(:,y,z)) ./ 45;
    end
end

signal = signal ./ noise;

%% Capacity
    
SNR = 1;
    
cpacity = (1/2) * log2(1 + (signal .* SNR));

for y = 1:90
    for z = 1:345
        capacity(y,z) = sum(cpacity(:,y,z));
    end
end
% save('capacity.mat','capacity');

harrison_max = max(max(capacity(36:65,65:98)));
% [X Y] = find(capacity(36:65,65:98) == harrison_max);
% max_X = X + 36 - 1;
% max_Y = Y + 65 - 1;

%% Secrecy Capacity 

for x = 1:90
    for y = 1:345
        secrecy_capacity(x,y) = harrison_max - capacity(x,y);
        if (secrecy_capacity(x,y) < 0)
            secrecy_capacity(x,y) = 0;
        end
    end
end
% save('secrecy_capacity.mat','secrecy_capacity');
I = imread('ClydeGIMPnoRXshowRooms.png');
figure();
imshow(I);
hold on
text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
hm = imagesc(secrecy_capacity);
set(hm,'AlphaData',~isnan(secrecy_capacity));
colormap(jet);
q = colorbar;
q.Position = [.855 .4 .016 .3];
ylabel(q, 'bits per channel use');
hm.XData = [36; 380];
hm.YData = [49; 139];
hold off
cf = gcf;
cf.PaperSize = [5 4];
cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];

SmalleyMin = min(min(secrecy_capacity(34:65,123:148)));
ConferenceMin = min(min(secrecy_capacity(5:65,1:61)));
CamachoMin = min(min(secrecy_capacity(34:65,154:179)));
ChambersMin = min(min(secrecy_capacity(34:65,213:240)));
HarrisonMin = min(min(secrecy_capacity(36:65,65:98)));
% clear capacity;
% clear cpacity;
% clear secrecy_capacity;
% clear harrison_max;
% clear noise;



% figure();
% plot(multiplier, SmalleyMin);
% hold on
% plot(multiplier, ConferenceMin);
% plot(multiplier, CamachoMin);
% plot(multiplier, ChambersMin);
% hold off
% legend('Smalley','Conference','Camacho','Chambers');
