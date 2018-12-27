clear;
close all;

load truncated_linear_signal.mat;

%% Capacity
noise = .01;

cpacity = (1/2) * log2(1 + (truncated_linear_signal / noise));

for y = 1:90
    for z = 1:345
        capacity(y,z) = sum(cpacity(:,y,z));
    end
end
save('capacity.mat','capacity');

harrison_max = max(max(capacity(36:65,65:98)));
[X Y] = find(capacity(36:65,65:98) == harrison_max);
max_X = X + 36 - 1;
max_Y = Y + 65 - 1;

%% Secrecy Capacity 

for x = 1:90
    for y = 1:345
        secrecy_capacity(x,y) = harrison_max - capacity(x,y);
        if (secrecy_capacity(x,y) < 0)
            secrecy_capacity(x,y) = 0;
        end
    end
end
save('secrecy_capacity.mat','secrecy_capacity');
