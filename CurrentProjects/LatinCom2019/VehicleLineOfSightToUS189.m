%X values represent the points of line of site in seconds 
X = [527 552 552 566 566 568.3 568.3 571 571 575 575 581 581 584.8 584.8 596 596 603 603 611 611 629 629 656 656 661 661 680 680 682 682 722 722 732 732 771.5 771.5 805 805];

X1 = X - 527; %We subtract 527 because we start the trip back to the US 189 at 527s in the video

Y1 = [1 1  0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0];
% Y Values represent when we have line of sight corresponding to the X times. 1 means we have line of
% sight, 0 means we do not.

%Here we plot the actual data:
figure()
plot(X1, Y1, 'LineWidth',2);
ylim([0 1.5]); % Y doesnt ever exceed 1; limit for aesthetic/readability
xlim([0 300]); % X values never exceed 300; same reason above
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');
