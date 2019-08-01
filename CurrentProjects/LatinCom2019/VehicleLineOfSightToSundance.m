%X1 values represent points where we had line of sight in seconds
X1 = [0.00 43 43 80 80 94.3 94.3 134 134 139 139 192 192 205.43 205.43 210 210 212.3 212.3];

Y1 = [1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0]; %1 means we had line of sight, and 0 means we did not, corresponding to the X1 value times

%Here we plot the data
figure()
plot(X1,Y1);
ylim([0 1.5]); % Y doesnt ever exceed 1; limit for aesthetic/readability
xlim([0 300]); % X values never exceed 300; same reason above 
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');