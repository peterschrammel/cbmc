public class Main {
    public void replaceNondetAssignment() {
        Object temp = org.cprover.CProver.nondetWithoutNull(new Object());
    }

    public void replaceNondetAssignmentUnbox() {
        int temp = org.cprover.CProver.nondetWithoutNull(new Integer());
    }

    public void replaceNondetAssignmentImplicitCast() {
        Integer temp = org.cprover.CProver.nondetWithoutNull(new Integer());
    }

    public void replaceNondetAssignmentExplicitCast() {
        Integer temp = (Integer)org.cprover.CProver.nondetWithoutNull(new Integer());
    }

    public void replaceNondetAssignmentAddition() {
        int temp = ((int)org.cprover.CProver.nondetWithoutNull(new Integer())) + 3;
    }

    public void replaceNondetUnused() {
        org.cprover.CProver.nondetWithoutNull(new Object());
    }

    public int replaceNondetReturnUnboxed() {
        return org.cprover.CProver.nondetWithoutNull(new Integer());
    }

    public Object replaceNondetReturn() {
        return org.cprover.CProver.nondetWithoutNull(new Object());
    }

    public Integer replaceNondetReturnWithImplicitCast() {
        return org.cprover.CProver.nondetWithoutNull(new Integer());
    }

    public Integer replaceNondetReturnWithExplicitCast() {
        return (Integer)org.cprover.CProver.nondetWithoutNull(new Integer());
    }

    public Integer replaceNondetReturnAddition() {
        return ((int)org.cprover.CProver.nondetWithoutNull(new Integer())) + 3;
    }
}
