clear;
close all;

load linear_signal.mat;
load linear_noisefloor.mat;
signal = linear_signal;
% noise = linear_noisefloor;

%% Capacity
multiplier = linspace(.001, 1, 100);
for index = 1:100
    noise = linear_noisefloor;
    noise = noise .* multiplier(index);
    
cpacity = (1/2) * log2(1 + (signal ./ noise));

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

SmalleyMin(index) = min(min(secrecy_capacity(34:65,123:148)));
ConferenceMin(index) = min(min(secrecy_capacity(5:65,1:61)));
CamachoMin(index) = min(min(secrecy_capacity(34:65,154:179)));
ChambersMin(index) = min(min(secrecy_capacity(34:65,213:240)));
clear capacity;
clear cpacity;
clear secrecy_capacity;
clear harrison_max;
clear noise;
end

figure();
plot(multiplier, SmalleyMin);
hold on
plot(multiplier, ConferenceMin);
plot(multiplier, CamachoMin);
plot(multiplier, ChambersMin);
hold off
legend('Smalley','Conference','Camacho','Chambers');
