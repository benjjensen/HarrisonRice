%This data comes from the trip up Diamond Fork (not on the way back)
% X1 values represent points where we had line of sight in seconds
X1 = [0 20 20 269 269 271 271 299 299 307 307 312 312 336 336];

Y1 = [1 1 0 0 1 1 0 0 1 1 0 0 1 1 0]; %1 means we had line of sight, and 0 means we did not, corresponding to the X1 value times

%Here we plot the data
figure()
plot(X1,Y1);
ylim([0 1.5]); % Y doesnt ever exceed 1; limit for aesthetic/readability
xlim([0 350]); % X values never exceed 350; same reason above 
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');
title('Line Of Sight Up Diamond Fork');


%Data on the way back:
% X2 values represent points where we had line of sight in seconds
X2 = [0 55 55 60 60 61 61 87 87 88 88 342];

Y2 = [1 1 0 0 1 1 0 0 1 1 0 0]; %1 means we had line of sight, and 0 means we did not, corresponding to the X1 value times

%Here we plot the data
figure()
plot(X2,Y2);
ylim([0 1.5]); % Y doesnt ever exceed 1; limit for aesthetic/readability
xlim([0 350]); % X values never exceed 350; same reason above 
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');
title('Line Of Sight Diamond Fork: Return');
