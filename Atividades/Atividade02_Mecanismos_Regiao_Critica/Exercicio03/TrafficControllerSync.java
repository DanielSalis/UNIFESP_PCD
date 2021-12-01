public class TrafficControllerSync {

    private boolean isBridgeFree = true;

    public void SetBridgeState(boolean value) {
        this.isBridgeFree = value;
    }

    public boolean GetBridgeState() {
        return this.isBridgeFree;
    }

    public synchronized void crossBridge() {
        while (this.GetBridgeState() == false) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println(e);
                break;
            }
        }
        SetBridgeState(false);
        notifyAll();
    }

    public synchronized void exitBridge() {
        this.SetBridgeState(true);
        notifyAll();
    }

    public void enterLeft() {
        crossBridge();
    }

    public void enterRight() {
        crossBridge();
    }

    public void leaveLeft() {
        exitBridge();
    }

    public void leaveRight() {
        exitBridge();
    }

}