A <- c(0.541741, 0.304412, 0.508857, 0.487885)
              
pdf("eval9.pdf", width=6/2.54, height=10/2.54,
    pointsize=10)
par(mgp=c(2.2,0.45,0), tcl=-0.4, mar=c(3.3,3.6,1.1,1.1))

barplot(A,
        names.arg=c(0, 1, 2, 3),
        xlab="Room Number",
        ylab="Average of Metrics",
        col=c("magenta", "blue", "orange", "red"),
        beside=TRUE,
        ylim=c(0,1)
        )
legend(x=2.5, y=1.0,
       c("room 0", "room 1", "room 2", "room 3"),
       cex=1,
       title="Evaluation",
       col=c("magenta", "blue", "orange", "red"),
       pch=c(22,0,0)
)
dev.off()
