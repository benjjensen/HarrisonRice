%This data comes from the AlpineLoop2 Video on Box
% X1 values represent points where we had line of sight in seconds
X2 = [0 27 27 67 67 87 87 90 90 94 94 105 105 113 113 118 118 146 146 149 149 153 153 157 157];

Y2 = [0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0]; %1 means we had line of sight, and 0 means we did not, corresponding to the X1 value times

%Here we plot the data
figure()
plot(X2,Y2);
ylim([0 1.5]); % Y doesnt ever exceed 1; limit for aesthetic/readability
xlim([0 160]); % X values never exceed 160; same reason above 
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');
title('Line Of Sight Alpine Loop 1');

%This data comes from the AlpineLoop3 Video on Box
% X1 values represent points where we had line of sight in seconds
X2 = [0 41 41 44 44 85 85 93 93 110 110 118 118];

Y2 = [0 0 1 1 0 0 1 1 0 0 1 1 0]; %1 means we had line of sight, and 0 means we did not, corresponding to the X1 value times

%Here we plot the data
figure()
plot(X2,Y2);
ylim([0 1.5]); % Y doesnt ever exceed 1; limit for aesthetic/readability
xlim([0 160]); % X values never exceed 160; same reason above 
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');
title('Line Of Sight Alpine Loop 2');

