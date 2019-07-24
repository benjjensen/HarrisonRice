%These are all of the points where we had line of sight in seconds. 1 means
%we had sight
X1 = [0.00 43 43 80 80 94.3 94.3 134 134 139 139 192 192 205.43 205.43 210 210 212.3 212.3];
Y1 = [1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0];

figure()
plot(X1,Y1);
ylim([0 1.5]);
xlim([0 300]);
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');