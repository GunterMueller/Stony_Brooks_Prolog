;;; -*-Mode:emacs-lisp-*-

;;--for some reason, @sp is documented but not implemented in my version
(put 'sp 'texinfo-format 'texinfo-format-sp)
(defun texinfo-format-sp ()
  (let ((arg (car (read-from-string (texinfo-parse-arg-discard)))))
    (insert-char ?\n arg)))

;;--@alphanumerate: like @enumerate but uses letters instead of numbers
(put 'alphanumerate 'texinfo-item 'texinfo-alphanumerate-item)
(defun texinfo-alphanumerate-item ()
  (texinfo-discard-line)
  (let ((next (1+ (car (cdr (car texinfo-stack))))))
    (setcar (cdr (car texinfo-stack)) next)
    (insert ?\b (format "  %c. " next) ?\n))
  (forward-line -1))

(put 'alphanumerate 'texinfo-format 'texinfo-alphanumerate)
(defun texinfo-alphanumerate ()
  (texinfo-push-stack 'alphanumerate (1- ?a))
  (setq fill-column (- fill-column 5))
  (texinfo-discard-line))

(put 'alphanumerate 'texinfo-end 'texinfo-end-alphanumerate)
(defun texinfo-end-alphanumerate ()
  (setq fill-column (+ fill-column 5))
  (texinfo-discard-command)
  (let ((stacktop
	 (texinfo-pop-stack 'alphanumerate)))
    (texinfo-do-itemize (nth 1 stacktop))))

;;--@REFILL: kind of like @refill but justifies too
(put 'REFILL 'texinfo-format 'texinfo-format-REFILL)
(defun texinfo-format-REFILL ()
  (texinfo-discard-command)
  (fill-paragraph t))

;;--@bb: like @b but uses quote in info
(put 'bb 'texinfo-format 'texinfo-format-code)

;;--@SP{n} inserts n spaces
(put 'SP 'texinfo-format 'texinfo-format-SP)
(defun texinfo-format-SP ()
  (insert-char ?  (car (read-from-string (texinfo-parse-arg-discard)))))

;;--@center used to center its argument before expanding it.
;;--as a result, the centering was sometimes off. this definition
;;--performs the expansion first.
(defun texinfo-format-center ()
  (texinfo-discard-command)
  (texinfo-format-expand-region
   (progn (beginning-of-line) (point))
   (progn (end-of-line) (point)))
  (let ((indent-tabs-mode nil))
    (center-line)))

;;--@ENTRY{left}{right} expands `left' and `right' and then
;;--flushes them to either side by filling the middle with dots.
;;--like entries in the table of contents.
(put 'ENTRY 'texinfo-format 'texinfo-format-ENTRY)
(defun texinfo-format-ENTRY ()
  (let ((arg1 (texinfo-parse-expanded-arg))
	(arg2 (texinfo-parse-expanded-arg))
	n m)
    (texinfo-discard-command)
    (setq arg1 (concat fill-prefix arg1))
    (setq n (- fill-column (length arg1) (length arg2)))
    (insert arg1)
    (setq m (% (current-column) 3))
    (cond ((= m 1) (insert-char ?  2) (setq n (- n 2)))
	  ((= m 2) (insert-char ?  1) (setq n (1- n))))
    (while (>= n 3)
      (insert " . ")
      (setq n (- n 3)))
    (if (> n 0) (insert-char ?  n))
    (insert arg2)))

;;--@B{arg} is a noop for info. in TeX it places its argument
;;--in a box of predefined size. this is just a hack for the
;;--manual.
(put 'B 'texinfo-format 'texinfo-discard-command)

;;--because I need to expand the argument to certain commands
;;--like @center first (before invoking the command),
;;--texinfo-format-parse-args must sometimes work in a region
;;--narrowed to encompass only 1 argument. therefore, it can't
;;--do (forward-char 1) if it is at the end of the narrowed
;;--region.
(defun texinfo-format-parse-args ()
  (let ((start (1- (point)))
	next beg end
	args)
    (search-forward "{")
    (while (/= (preceding-char) ?\})
      (skip-chars-forward " \t\n")
      (setq beg (point))
      (re-search-forward "[},]")
      (setq next (point))
      (forward-char -1)
      (skip-chars-backward " \t\n")
      (setq end (point))
      (cond ((< beg end)
	     (goto-char beg)
	     (while (search-forward "\n" end t)
	       (replace-match " "))))
      (setq args (cons (if (> end beg) (buffer-substring beg end))
		       args))
      (goto-char next))
    (if (and (not (eobp)) (eolp)) (forward-char 1)) ;;--be careful
    (setq texinfo-command-end (point))
    (nreverse args)))

;;--@CENTER: just like @center, except that in TeX it does a \par
;;--first.
(put 'CENTER 'texinfo-format 'texinfo-format-center)
