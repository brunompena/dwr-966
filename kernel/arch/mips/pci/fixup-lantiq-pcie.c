/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C)2009~2013 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 */
 /*!
 \file fixup-lantiq-pcie.c
 \ingroup PCIE
 \brief PCIe Fixup functions source file
*/
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/pci_ids.h>

#include <lantiq_soc.h>

#include "pcie-lantiq.h"

#define PCI_VENDOR_ID_INFINEON          0x15D1
#define PCI_DEVICE_ID_INFINEON_PCIE     0x0011

#define PCI_VENDOR_ID_LANTIQ            0x1BEF
#define PCI_DEVICE_ID_LANTIQ_PCIE       0x0011

static void pcie_fixup_resource(struct pci_dev *dev)
{
	u32 reg;
#ifdef LTQ_PCIE_DBG
	struct ltq_pcie_controller *ctrl = dev->bus->sysdata;
	int rc_port = ctrl->port;
#endif /* LTQ_PCIE_DBG */
	pcie_dbg(PCIE_MSG_FIXUP, "%s port %d dev %s: enter\n",
		__func__, rc_port, pci_name(dev));

	pcie_dbg(PCIE_MSG_FIXUP,
		"%s port %d : fixup host controller %s (%04x:%04x)\n",
		__func__, rc_port, pci_name(dev), dev->vendor, dev->device);

	/* Setup COMMAND register */
	reg = PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER	/* |
		PCI_COMMAND_INTX_DISABLE */ | PCI_COMMAND_SERR;
	pci_write_config_word(dev, PCI_COMMAND, reg);
	pcie_dbg(PCIE_MSG_FIXUP, "%s port %d dev %s: exit\n",
		__func__, rc_port, pci_name(dev));
}

DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_INFINEON, PCI_DEVICE_ID_INFINEON_PCIE,
			pcie_fixup_resource);
DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_LANTIQ, PCI_DEVICE_ID_LANTIQ_PCIE,
			pcie_fixup_resource);

static void pcie_rc_class_early_fixup(struct pci_dev *dev)
{
	struct ltq_pcie_controller *ctrl = dev->bus->sysdata;
	int rc_port = ctrl->port;

	pcie_dbg(PCIE_MSG_FIXUP, "%s port %d dev %s: enter\n",
		__func__, rc_port, pci_name(dev));

	/* G.HN also has wrong class code */
	if (dev->devfn == PCI_DEVFN(0, 0)
		&& (((dev->class >> 8) == PCI_CLASS_BRIDGE_HOST)
		|| ((dev->class >> 8) == PCI_CLASS_NETWORK_OTHER))) {
		dev->class =
			(PCI_CLASS_BRIDGE_PCI << 8) | (dev->class & 0xff);
		pr_info("%s port %d: fixed pci host bridge to pci-pci bridge\n",
			__func__, rc_port);
	}
	pcie_dbg(PCIE_MSG_FIXUP, "%s port %d dev %s: exit\n",
		__func__, rc_port, pci_name(dev));
}

DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_INFINEON, PCI_DEVICE_ID_INFINEON_PCIE,
			pcie_rc_class_early_fixup);

DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_LANTIQ, PCI_DEVICE_ID_LANTIQ_PCIE,
			pcie_rc_class_early_fixup);
